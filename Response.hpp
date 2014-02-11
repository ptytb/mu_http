#pragma once

#include "Page.hpp"
#include "FileReader.hpp"
#include "HttpHeaders.hpp"
#include "HttpServerLine.hpp"

#include <memory>

class Response
{
	public:
		virtual ~Response() {};

		virtual bool hasData() const = 0;
		virtual const char *data() const = 0;
		virtual size_t blockSize() const = 0;
		virtual void move(size_t delta) = 0;

		HttpServerLine sl;
		HttpHeaders hh;
};

class ResponsePage : public Response
{
	public:
		ResponsePage(Page *page);
		virtual ~ResponsePage();
		virtual bool hasData() const;
		virtual const char *data() const;
		virtual size_t blockSize() const;
		virtual void move(size_t delta);

	protected:
		std::string content;
		size_t current;
		std::unique_ptr<Page> page;
};

#define READ_FILE_BUFFER_LEN 2048

class ResponseFile : public Response
{
	public:
		ResponseFile(std::string path, size_t length);
		virtual ~ResponseFile() { }
		virtual bool hasData() const;
		virtual const char *data() const;
		virtual size_t blockSize() const;
		virtual void move(size_t delta);

	protected:
		std::string head;
		bool sendingHttpHead;
		size_t length;
		size_t offset; /* В head или buffer */
		size_t fileBytesInBuffer;
		size_t fileBytesRead;
		FileReader fileReader;
		char buffer[READ_FILE_BUFFER_LEN];
};


