/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#include "PageError.hpp"

#include <sstream>

std::string PageError::getContentHtml()
{
	std::ostringstream cs;
	switch (code) {
		case 403: 
			description = "Доступ запрещен";
			details = "Сюда нельзя.";
			break;
		case 404:
			description = "Ресурс не существует";
			details = "Поищите где-нибудь ещё.";
			break;
		case 501:
			description = "Запрашиваемый метод не реализован";
			details = "Пока я так не умею.";
			break;
		case 400:
			description = "Некорректный запрос";
			details = "Вы отправили некорректный запрос";
			break;
	}

	cs << "<h4>" << code << " " << description << "</h4>";
	cs << "<br>";
	cs << details;
	return cs.str();
}


