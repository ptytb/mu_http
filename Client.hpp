/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#pragma once

#include <netinet/in.h>
#include <map>
#include <string>
#include <queue>
#include <memory>

#include "AsyncIO.hpp"
#include "Page.hpp"
#include "HttpHeaders.hpp"
#include "HttpServerLine.hpp"
#include "Response.hpp"
#include "Request.hpp"

#define CLIENT_MAX_WAIT 5
#define CLIENT_MAX_REQUEST_LENGTH 16384
#define CLIENT_REQUEST_CHUNK 16384
#define CLIENT_FILE_SEND_BUF 900

class ClientInfo
{
	public:
		ClientInfo(int socket, sockaddr_in addr):
			socket(socket),
			addr(addr)
		{ }

		int socket;
		sockaddr_in addr;
};

class Client
{
	public:
		static void *start(void *args);

	private:
		static Response *getResponse(Request &request);
		void loop();
		void process(Request& request);

		Client(ClientInfo cinfo);
		~Client();

		AsyncIO aio; 
		std::queue<Response*> responseQueue;
		ClientInfo cinfo;
		size_t requestsHandled;
};


