#pragma once

#include <string>
#include <map>

#include "HttpClientLine.hpp"
#include "HttpHeaders.hpp"

enum Method
{
	GET, POST
};

class Request
{
	public:
		static Request parse(std::string &text);

		bool valid;
		bool hasBody;
		int error;

		Method method;
		std::string path;
		std::string body;

		/* Основные заголовки */

		std::string host;
		std::string referer;
		std::string userAgent;
		std::string transferEncoding;

		/* Заголовки сущности */

		size_t contentLength;
};

