/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#include "HttpServerLine.hpp"

#include <sstream>

/*
HttpServerLine(int code) :
	code(code)
{ } */

std::string HttpServerLine::toString()
{
	std::ostringstream es;
	es << "HTTP/1.1 ";
	es << code;
	es << " ";

	switch (code)
	{
		case 200: es << "OK"; break;
		case 400: es << "Bad Request"; break;
		case 403: es << "Forbidden"; break;
		case 404: es << "Not Found"; break;
		case 405: es << "Method Not Allowed"; break;
		case 411: es << "Length Required"; break;
		case 501: es << "Not Implemented"; break;
		case 505: es << "HTTP Version Not Supported"; break;
	}

	return es.str();
}


