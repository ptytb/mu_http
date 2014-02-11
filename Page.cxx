/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#include "Page.hpp"

#include <sstream>
#include <cstdlib>
#include <string.h>

#include "ustring.hpp"

std::string Page::getCellarHtml()
{
	time_t t;
	std::ostringstream sc;

	time(&t);

	sc << stime("%a %F %T", &t);
	sc << "<br>&mu; Http Сервер (c) Пронин Илья, 2013";
	return sc.str();
}

std::string Page::getHeadHtml()
{
	std::ostringstream sh;
	sh << "<head>";
	sh << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=";
	sh << charset;
	sh << "\">";
	sh << "<link href=\"/" + stylesheet + "\" rel=\"stylesheet\" type=\"text/css\">";
	sh << "<title>" + title + "</title>";
	sh << "</head>";
	return sh.str();
}

std::string Page::getBodyHtml()
{
	std::ostringstream sb;
	sb << "<body>";
	sb << "<h1>" + message + "</h1>";
	sb << getContentHtml();
	sb << "<span id=\"cellar\">" + getCellarHtml() + "</span>";
	sb << "</body>";
	return sb.str();
}

std::string Page::getHtml()
{
	return "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\
      \"http://www.w3.org/TR/html4/strict.dtd\">\n\
<html>\n"
		+ getHeadHtml() + getBodyHtml() + "</html>";
}


