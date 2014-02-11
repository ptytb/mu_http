/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#pragma once

#include <string>

class HttpServerLine
{
	public:
		int code;

		HttpServerLine() { };
	//	HttpServerLine(int code);
		std::string toString();
};


