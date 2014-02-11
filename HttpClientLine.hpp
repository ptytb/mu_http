/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#pragma once

#include <string>
#include <map>

class HttpClientLine
{
	public:
		bool valid;

		std::string method;
		std::string uri;
		int httpHigh;
		int httpLow;
		std::map<std::string, std::string> query;

		static HttpClientLine parse(std::string &httpReq);
};


