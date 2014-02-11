/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#pragma once

#include "Page.hpp"

class PageError : public Page
{
	public:
		int code;
		std::string description;
		std::string details;

		virtual std::string getContentHtml();
};


