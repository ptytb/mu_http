/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#include "HttpHeaders.hpp"

#include <sstream>
#include <algorithm>

std::string HttpHeaders::toString()
{
	std::ostringstream sh;
	std::map<std::string, std::string>::iterator i;
	for (i = headers.begin(); i != headers.end(); i++) {
		sh << i->first;
		sh << ": ";
		sh << i->second;
		sh << "\r\n";
	}
	return sh.str();
}

HttpHeaders HttpHeaders::parse(std::string &httpReq)
{
	HttpHeaders hh;
	hh.valid = false;

	size_t pos = httpReq.find("\r\n");

	if (pos == std::string::npos)
		goto finish;

	for (;;)
	{
		pos += 2;

		if (pos + 1 >= httpReq.length())
			goto finish;
		
		if (httpReq.at(pos) == '\r' && httpReq.at(pos + 1) == '\n')
		{
			break;
		}

		size_t delim = httpReq.find(':', pos);

		if (delim == std::string::npos)
			goto finish;

		size_t delimEnd = delim + 1;

		while ((delimEnd < httpReq.length()) &&
				isspace(httpReq.at(delimEnd)))
		{
			++delimEnd;
		}

		if (delimEnd >= httpReq.length())
			goto finish;

		size_t hNextBeg = httpReq.find("\r\n", delimEnd);

		if (hNextBeg == std::string::npos)
			goto finish;

		std::string key = httpReq.substr(pos, delim - pos);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);

		hh.headers[key] = 
			httpReq.substr(delimEnd, hNextBeg - delimEnd);

		pos = hNextBeg;
	}

	hh.valid = true;

finish:
	return hh;
}

bool HttpHeaders::exists(std::string key)
{
	return headers.find(key) != headers.end();
}

std::string &HttpHeaders::operator[](const std::string &key)
{
	return headers[key];
}


