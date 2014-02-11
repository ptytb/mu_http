/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#include "Log.hpp"

#include "cstdlib"
#include <stdio.h>
#include "ustring.hpp"
#include <stdarg.h>

void log(std::string msg, ...)
{
	time_t t;
	va_list ap;

	time(&t);
	msg.insert(0, stime("%T ", &t));
	msg += '\n';
	
	va_start(ap, msg);
	vprintf(msg.c_str(), ap);
	va_end(ap);
}

