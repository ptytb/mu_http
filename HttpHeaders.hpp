/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#pragma once

#include <map>
#include <string>

class HttpHeaders
{
	public:
		std::map<std::string, std::string> headers;
		bool valid;

		bool exists(std::string key);
		std::string toString();
		std::string &operator[](const std::string &key);

		static HttpHeaders parse(std::string &httpReq);
};


