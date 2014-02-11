/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#include "PageBrowse.hpp"
#include "DirReader.hpp"

#include <sstream>

#include "ustring.hpp"
#include "main.hpp"
#include "Log.hpp"

std::string PageBrowse::getContentHtml()
{
	std::ostringstream cs;
	cs << "<table>";
	cs << "<tr>";
	cs << "<td></td>";
	cs << "<td><h4>Имя</h4></td>";
	cs << "<td><h4>Размер</h4></td>";
	cs << "<td><h4>Изменен</h4></td>";
	cs << "</tr>";

	if (uri != "")
	{
		cs << "<tr>";
		cs << "<td id=\"iconFolder\">";
		cs << "</td>";

		cs << "<td>";
		cs << "<a href=\"";
		std::string parent = DirReader::extractParent(uri);
		cs << parent;
		cs << "\">";
		cs << "..";
		cs << "</a>";
		cs << "</td>";
		cs << "</tr>";
	}

	std::map<int, int>::iterator i, end;
	dirReader.getDir(uri, i, end);

	for (; i != end; ++i)
	{
		FileInfo& meta = dirReader.getMeta(i->second);

		cs << "<tr>";

		cs << "<td id=\"";
		cs << (meta.isDir ? "iconFolder" : "iconFile");
		cs << "\">";
		cs << "</td>";

		cs << "<td>";
		cs << "<a href=\"";
	
		std::string fullUri = uri + (uri == "/" ? "" : "/") + meta.name;
		std::string encFullUri = uriEncode(fullUri);

		cs << encFullUri;
		cs << "\">";
		cs << meta.name;
		cs << "</a>";
		cs << "</td>";

		cs << "<td>";
		if (!meta.isDir)
		{
			cs << meta.length;
		}
		cs << "</td>";

		cs << "<td>";
		cs << stime("%a %F %T", &(meta.modified));
		cs << "</td>";

		cs << "</tr>";
	}

	cs << "</table>";
	return cs.str();
}


