/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#include "HttpClientLine.hpp"

#include <cstdlib>
#include <cstdio>
#include <sstream>
#include "ustring.hpp"

HttpClientLine HttpClientLine::parse(std::string &httpReq)
{
	HttpClientLine cl;

	cl.valid = false;
	size_t beg, end;
	std::string line;

	end = httpReq.find("\r\n");

	if (end == std::string::npos)
		goto finish;

	line = httpReq.substr(0, end);

	end = line.find(' ');

	if (end == std::string::npos)
		goto finish;

	cl.method = line.substr(0, end);

	beg = end + 1;

	if (beg >= line.length())
		goto finish;
	
	end = line.find_first_of("?# ", beg);

	if (end == std::string::npos)
		goto finish;

	cl.uri = line.substr(beg, end - beg);

	beg = end + 1;

	if (beg >= line.length())
		goto finish;

	if (line.at(end) == '?')
	{
		end = line.find_first_of("# ", beg);

		if (end == std::string::npos)
			goto finish;

		parseArgs(line.substr(beg, end - beg), cl.query);

		beg = end;
	}
	
	beg = line.find("HTTP/", beg);

	if (beg == std::string::npos)
		goto finish;

	beg += 5;

	if (beg + 2 >= line.length())
		goto finish;

	cl.httpHigh = line[beg] - '0';
	cl.httpLow = line[beg + 2] - '0';

	cl.valid = true;

finish:
	return cl;
}


