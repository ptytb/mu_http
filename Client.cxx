/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#include "Client.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "ustring.hpp"
#include "PageBrowse.hpp"
#include "PageError.hpp"
#include "HttpHeaders.hpp"
#include "HttpClientLine.hpp"
#include "HttpServerLine.hpp"
#include "Log.hpp"
#include "FileReader.hpp"
#include "PageFactory.hpp"
#include "main.hpp"

Client::Client(ClientInfo cinfo) :
	aio(cinfo.socket, 2048, 2048),
	cinfo(cinfo),
	requestsHandled(0)
{ }

void *Client::start(void *args)
{
	ClientInfo cinfo(* static_cast<ClientInfo*> (args));
	delete static_cast<ClientInfo*> (args);

	try
	{
		Client client(cinfo);

		client.loop();
	}
	catch (std::exception& e)
	{
		log("Не удалось создать сеанс: %s", e.what());
		close(cinfo.socket);
	}

	return NULL;
}

void Client::process(Request& request)
{
	Response* response = getResponse(request); 
	responseQueue.push(response); 
	++requestsHandled; 
}

void Client::loop()
{
	bool readingBody = false;
	bool flushResponsesAndDie = false;
	size_t bodyOffset = 0;

	std::string text;
	Request request;

	aio.setTimeout(atol(config["server.timeout"].c_str()));

	// log("Подключение от %s", inet_ntoa(cinfo.addr.sin_addr));

	for (;;)
	{		
		/* Чтение частей нового запроса */

		size_t rdc = aio.readCount();
		while (rdc)
		{
			char chunk[CLIENT_REQUEST_CHUNK];
			size_t chunkLength = aio.read(chunk,
					CLIENT_REQUEST_CHUNK);
			text.append(chunk, chunkLength);
			rdc -= chunkLength;
			//	log("read chunk: %d\n", chunkLength);
		}

		/* Формирование запросов */

		if (!readingBody)
		{
			/* Сформирован заголовок нового запроса */

			if ((bodyOffset = text.find("\r\n\r\n"))
					!= std::string::npos)
			{
				//	log("incoming request: ");
				//	log(text);

				request = Request::parse(text);
				text.erase(0, bodyOffset + 4);
				readingBody = request.hasBody;

				if (!readingBody)
				{
					process(request);

					if (!request.valid)
					{
						/* Плохой запрос.
						 * Идем на цикл сброса очереди ответов */

						flushResponsesAndDie = true;
						goto flush_responses;
					}
				}

			}
		}

		if (readingBody)
		{
			/* Загружено тело запроса */

			if (text.length() >= request.contentLength)
			{
				readingBody = false;
				request.body = text.substr(0,
						request.contentLength);
				text.erase(0, request.contentLength + 4);
				process(request);
			}
		}

flush_responses:

		if (!responseQueue.empty())
		{
			Response* response = responseQueue.front();

			if (response->hasData())
			{
				int wrc = aio.write(response->data(),
						response->blockSize());
				response->move(wrc);
				//	log("write chunk: %d\n", wrc);
			}
			else
			{
				//	log("Ответ удален");
				delete response;
				responseQueue.pop();

				if (!responseQueue.empty())
				{
					goto flush_responses;
				}
				else if (flushResponsesAndDie)
				{
					break;
				}
			}
		}

		int pr = aio.process();

		if (pr < 0)
		{
			break;
		}

		if (flushResponsesAndDie)
		{
			/* Внутренний подцикл */

			goto flush_responses;
		}

	} // for(;;)
}

Client::~Client()
{
	close(cinfo.socket);

	while (!responseQueue.empty())
	{
		delete responseQueue.front();
		responseQueue.pop();
	}

	// log("Сеанс завершен");
	// log("Обработано %d запросов\n", requestsHandled);	
}

Response *Client::getResponse(Request &request)
{
	if (!request.valid)
	{
		return new ResponsePage(PageFactory::error(request.error));
	}

	if (request.method == POST)
	{
		return new ResponsePage(PageFactory::post(request));
	}

	/* Ищу ресурс */

	try
	{
		FileInfo& fi = dirReader.getMeta(request.path);

		if (!fi.isDir)
		{
			/* Выслать файл */
			return new ResponseFile(dirReader.root + request.path,
					fi.length);
		}
		else
		{
			return new ResponsePage(PageFactory::browse(request.path));
		}
	}
	catch (std::exception& e)
	{
		return new ResponsePage(PageFactory::error(404));
	}
}


/*
   std::map<std::string, std::string>::iterator i = ch.headers.begin();
   for (; i != ch.headers.end(); i++) {
   log(i->first + " = " + i->second);
   }
   */
