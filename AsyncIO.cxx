#include "AsyncIO.hpp"

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

RingBuffer::RingBuffer(size_t size) :
	data(new char[size + 1]),
	begin(0),
	end(0),
	capacity(size + 1)
{
}

RingBuffer::~RingBuffer()
{
	delete [] data;
}

bool RingBuffer::isEmpty() const
{
	return begin == end;
}

bool RingBuffer::isFull() const
{
	return (end + 1) % capacity == begin;
}

size_t RingBuffer::straight() const
{
	return (begin <= end ? end : capacity) - begin;
}

size_t RingBuffer::gapStraight() const
{
	return (begin <= end)
		? capacity - end - (begin == 0)
		: begin - end - 1;
}

size_t RingBuffer::length() const
{
	return (begin <= end)
		? end - begin
		: capacity - (begin - end);
}

char *RingBuffer::peek()
{
	return data + begin;
}

char *RingBuffer::gap()
{
	return data + end;
}

void RingBuffer::chop(size_t len)
{
	begin = (begin + len) % capacity;
}

void RingBuffer::expand(size_t len)
{
	end = (end + len) % capacity;
}

size_t RingBuffer::get(char *buf, size_t len)
{
	size_t part = MIN(straight(), len);
	memcpy(buf, peek(), part);
	chop(part);
	return part;
}

size_t RingBuffer::put(const char *buf, size_t len)
{
	size_t part = MIN(gapStraight(), len);
	memcpy(gap(), buf, part);
	expand(part);
	return part;
}

AsyncIO::AsyncIO(int fd, size_t rBufSize, size_t wBufSize) :
	fd(fd),
	rBuf(rBufSize),
	wBuf(wBufSize)
{
	if (!setBlocking(false))
	{
		throw "Не могу установить сокет в асинхронный режим";
	}
}

AsyncIO::~AsyncIO()
{
	close(fd);
}

bool AsyncIO::setBlocking(bool blocking)
{
	int opts = fcntl(fd, F_GETFL);
	
	if (blocking)
		opts &= ~O_NONBLOCK;
	else
		opts |= O_NONBLOCK;

	return fcntl(fd, F_SETFL, opts) != -1;
}

void AsyncIO::setTimeout(long sec)
{
	timeout.tv_sec = sec;
	timeout.tv_usec = 0;
}

size_t AsyncIO::read(char *buf, size_t len)
{
	return rBuf.get(buf, len);
}

size_t AsyncIO::write(const char *buf, size_t len)
{
	return wBuf.put(buf, len);
}

size_t AsyncIO::readCount() const
{
	return rBuf.length();
}

int AsyncIO::process()
{
	fd_set rd, wr, er;

	FD_ZERO(&rd);
	if (!rBuf.isFull())
		FD_SET(fd, &rd);
	
	FD_ZERO(&wr);
	if (!wBuf.isEmpty())
		FD_SET(fd, &wr);
	
	FD_ZERO(&er);
	FD_SET(fd, &er);

	int sr = select(fd + 1, &rd, &wr, &er, &timeout);

	if (sr == 0)
	{
		/* Таймаут */
		return -1;
	}

	if (sr == -1)
	{
		/* Прервано сигналом или ошибка, закрываемся. */
		perror("select()");
		return -1;
	}

	if (FD_ISSET(fd, &rd))
	{
		int rn = recv(fd, rBuf.gap(), rBuf.gapStraight(), 0);
		if (rn <= 0)
		{
			/* Ошибка или клиент отключился */
			return -1;
		}
		rBuf.expand(rn);
	}

	if (FD_ISSET(fd, &wr))
	{
		int sn = send(fd, wBuf.peek(), wBuf.straight(), 0);
		if (sn <= 0)
		{
			return -1;
		}
		wBuf.chop(sn);
	}

	if (FD_ISSET(fd, &er)) {
		return -1;
	}

	return 0;
}


