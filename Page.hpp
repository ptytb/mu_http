/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#pragma once

#include <string>

class Page
{
	public:
		virtual ~Page() {};

		std::string stylesheet;
		std::string charset;
		std::string title;
		std::string message;

		virtual std::string getContentHtml() { return ""; }
		std::string getCellarHtml();

		std::string getHeadHtml();
		std::string getBodyHtml();
		
		std::string getHtml();
};


