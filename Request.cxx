#include "Request.hpp"
#include "ustring.hpp"

#include <sstream>
#include <cstdio>

Request Request::parse(std::string &text)
{
	Request request;
	request.valid = false;
	request.hasBody = false;

	HttpClientLine cl = HttpClientLine::parse(text);
	HttpHeaders ch;

	if (!cl.valid)
	{
		/* 400 Bad Request*/
		request.error = 400;
		goto finish;
	}

	if (cl.httpLow != 1 || cl.httpHigh != 1)
	{
		/* 505 HTTP Version Not Supported */
		request.error = 505;
		goto finish;
	}

	if (cl.method == "GET")
	{
		request.method = GET;
	}
	else if (cl.method == "POST")
	{
		request.method = POST;
	}
	else
	{
		/* 501 Not Implemented */
		request.error = 501;
		goto finish;
	}

	request.path = uriDecode(cl.uri);

	if (!utf8Check(request.path))
	{
		/* 400 Bad Request*/
		request.error = 400;
		goto finish;
	}
	
	ch = HttpHeaders::parse(text);
	
	if (!ch.valid)
	{
		/* 400 Bad Request*/
		request.error = 400;
		goto finish;
	}

	if (request.method == POST)
	{
		if (!ch.exists("content-length")
				&& !ch.exists("transfer-encoding"))
		{
			/* 411 Length Required */
			request.error = 411;
			goto finish;
		}
	}
	
	/* Читаю заголовки */

	if (ch.exists("host"))
	{
		request.host = ch["host"];
	}

	if (ch.exists("referer"))
	{
		request.referer = ch["referer"];
	}

	if (ch.exists("user-agent"))
	{
		request.userAgent = ch["user-agent"];
	}

	if (ch.exists("content-length"))
	{
		std::istringstream is(ch["content-length"]);
		is >> request.contentLength;
		request.hasBody = true;
	}

	if (ch.exists("transfer-encoding"))
	{
		request.userAgent = ch["transfer-encoding"];
		request.hasBody = true;
	}

	request.valid = true;

finish:
	return request;
}

