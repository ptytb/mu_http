#include "Response.hpp"

#include "ustring.hpp"
#include "PageError.hpp"

#include <cstdlib>
#include <string.h>
#include <sstream>
#include "main.hpp"

/* Страница */

ResponsePage::ResponsePage(Page *page) :
	current(0),
	page(page)
{
	PageError *pe = dynamic_cast<PageError*> (page);
	sl.code = pe ? pe->code : 200;

	std::string htmlContent = page->getHtml();

	std::ostringstream contentLength;
	contentLength << htmlContent.length();
	hh.headers["Content-Length"] = contentLength.str();
	hh.headers["Connection"] = "Keep-Alive";
//	hh.headers["Server"] = config["server.name"];

	content = sl.toString() + "\r\n" +
		hh.toString() + "\r\n" +
		htmlContent;
}

ResponsePage::~ResponsePage() { }

bool ResponsePage::hasData() const
{
	return current < content.length();
}

const char *ResponsePage::data() const
{
	return content.c_str() + current;
}

size_t ResponsePage::blockSize() const
{
	return content.length() - current;
}

void ResponsePage::move(size_t delta)
{
	current += delta;
}

/* Файл */

ResponseFile::ResponseFile(std::string path, size_t length) :
	sendingHttpHead(true),
	length(length),
	offset(0),
	fileBytesInBuffer(0),
	fileBytesRead(0),
	fileReader(path)
{
	sl.code = 200;

	std::ostringstream contentLength;
	contentLength << length;

	hh.headers["Content-Length"] = contentLength.str();
	hh.headers["Connection"] = "Keep-Alive";

	head = sl.toString() + "\r\n" +
		hh.toString() + "\r\n";
}

bool ResponseFile::hasData() const
{
	return fileBytesRead < length;
}

const char *ResponseFile::data() const
{
	if (!sendingHttpHead)
	{
		return buffer + offset;
	}
	else {
		return head.c_str() + offset;
	}
}

size_t ResponseFile::blockSize() const
{
	if (!sendingHttpHead)
	{
		return fileBytesInBuffer - offset;
	}
	else {
		return head.length() - offset;
	}
}

void ResponseFile::move(size_t delta)
{
	if (!sendingHttpHead)
	{
		offset += delta;
		fileBytesRead += delta;

		if (offset == fileBytesInBuffer) {
			fileBytesInBuffer = fileReader.read(buffer,
					READ_FILE_BUFFER_LEN);
			offset = 0;
		}
	}
	else {
		offset += delta;
		if (offset == head.length()) {
			sendingHttpHead = false;
			offset = 0;
			move(0);
		}
	}
}


