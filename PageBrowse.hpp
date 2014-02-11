/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#pragma once

#include "Page.hpp"
#include "DirReader.hpp"

class PageBrowse : public Page
{
	public:
		std::string path;
		std::string uri;
		virtual std::string getContentHtml();

};


