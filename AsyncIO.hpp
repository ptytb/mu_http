#pragma once

/**
 * Асинхронный ввод-вывод
 */

#include <cstdlib>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define AIO_OK 0
#define AIO_CANT_NONBLOCK 1
#define AIO_NOMEM 2

class RingBuffer
{
	public:
		RingBuffer(size_t size);
		~RingBuffer();

		bool isEmpty() const;
		bool isFull() const;
		size_t straight() const;
		size_t gapStraight() const;
		size_t length() const;
		char *peek();
		char *gap();
		void chop(size_t len);
		void expand(size_t len);
		size_t get(char *buf, size_t len);
		size_t put(const char *buf, size_t len);

	protected:
		char *data;
		size_t begin;
		size_t end;
		size_t capacity;
};

class AsyncIO
{
	public:
		AsyncIO(int fd, size_t rBufSize, size_t wBufSize);
		~AsyncIO();

		bool setBlocking(bool blocking);
		void setTimeout(long sec);

		int process();

		size_t read(char *buf, size_t len);
		size_t write(const char *buf, size_t len);
		size_t readCount() const;

	protected:
		int err;
		int fd;
		RingBuffer rBuf;
		RingBuffer wBuf;
		timeval timeout;
};


