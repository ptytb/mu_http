#include "PageFactory.hpp"

#include "HttpServerLine.hpp"
#include "HttpHeaders.hpp"
#include "PageBrowse.hpp"
#include "PageError.hpp"
#include "ustring.hpp"
#include "main.hpp"

Page *PageFactory::browse(std::string path)
{
	PageBrowse *page = new PageBrowse();
	page->path = config["server.root"];
	std::string uri = path;
	page->uri = uri;
	page->title = "Обзор файлов";
	page->message = config["template.directoryPage.message"];
	page->charset = "UTF-8";
	page->stylesheet = config["template.directoryPage.stylesheet"];

	return page;
}

Page *PageFactory::error(int code)
{
	PageError *page = new PageError;
	page->code = code;
	page->message = config["template.errorPage.message"];
	page->title = "Заголовок";
	page->charset = "UTF-8";
	page->stylesheet = config["template.errorPage.stylesheet"];

	return page;
}

Page *PageFactory::post(Request& request)
{
	Page *page = new Page();
	std::map<std::string, std::string> query;
	parseArgs(request.body, query);

	std::map<std::string, std::string>::iterator i = query.begin();
	for (; i != query.end(); i++) {
		page->message += i->first + " = " + i->second + "<br>";
	}
	
	page->charset = "UTF-8";
	return page;
}

