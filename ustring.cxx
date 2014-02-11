/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#include "ustring.hpp"

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <cctype>
#include <cstdio>

std::string stime(std::string fmt, const time_t *t)
{
	tm tt;
	localtime_r(t, &tt);

	size_t len, max = 32;
	char *buf = (char*) malloc(max * sizeof(char));
	while ((len = strftime(buf, max, fmt.c_str(), &tt)) == 0) {
		max *= 2;
		buf = (char*) realloc(buf, max * sizeof(char));
	}	
	std::string result = buf;
	free(buf);
	return result;
}

std::string uriDecode(std::string encUri)
{
	std::stringstream dec;
	std::string::iterator i;
	int nquart = 0;
	bool readingHex = false;
	char outc = '\0';

	for (i = encUri.begin(); i != encUri.end(); i++) {
		char c = *i;

		switch (c) {
			case '%':
				readingHex = true;
				break;

			default:
				if (!readingHex) {
					dec << c;
				} else {
					outc <<= 4;
					c <= '9' ? c -= '0' : c = c - 'A' + 10;
					outc |= c;

					++nquart;

					if (nquart == 2)
					{
						if (outc)
							dec << outc;

						readingHex = false;
						nquart = 0;
						outc = '\0';
					}
				}
		}
	}

//	printf("%s\n", dec.str().c_str());
	return dec.str();
}

std::string uriEncode(std::string uri)
{
	static const char *reserved = "!*'();:@&=+$,/?%#[]";
	static const char *unreserved = "-._~"; // + ALPHA + DIGIT
	static const char *hex = "0123456789ABCDEF";

	std::ostringstream enc;
	std::string::iterator i;

	for (i = uri.begin(); i != uri.end(); i++) {
		unsigned char c = *i;
		if (isalnum(c) || index(unreserved, c) != NULL
				|| index(reserved, c) != NULL)
		{
			enc << c;
		} else {
			enc << '%';
			char second = hex[c % 16];
			c >>= 4;
			char first = hex[c % 16];
			enc << first << second;
		}
	}

	return enc.str();
}

void parseArgs(std::string queryString,
		std::map<std::string, std::string> &query)
{
	std::istringstream pairs(queryString);
	std::string s;

	while (std::getline(pairs, s, '&'))
	{
		std::istringstream pair(s);
		std::string key, value;

		std::getline(pair, key, '=');
		std::getline(pair, value, '=');

		query[uriDecode(key)] = uriDecode(value);
	}
}

bool utf8Check(std::string &s)
{
	std::string::iterator i;
	int bytesInChar = 0;
	int claimedBytes = 0;
	unsigned int value;

	for (i = s.begin(); i != s.end(); i++)
	{
		unsigned int c = (unsigned char) *i;
		
		/* NULL недопустим */

		if (!c)
		{
			return false;
		}

		/* Признак следующего символа */

		if ((c & 0xC0) == 0x80)
		{
			++bytesInChar;

			if (bytesInChar > claimedBytes)
			{
				/* Нашли мусор */
				return false;
			}

			value <<= 6;
			value |= 0x3F & c;

			if (bytesInChar == claimedBytes)
			{
				/* Проверка избыточности кода */

				switch (bytesInChar)
				{
					default:
					case 1:
						break;

					case 2:
						if (value < 0x80)
							return false;
						break;

					case 3:
						if (value < 0x800)
							return false;
						break;

					case 4:
						if (value < 0x10000
								|| value > 0x110000)
							return false;
						break;
				}
			}

			/* Идем за следующим байтом */

			continue;
		}
		
		if (bytesInChar < claimedBytes)
		{
			/* Неполный символ */
			return false;
		}

		/* Далее должны получить первый байт нового символа */

		if ((c & 0x80) == 0)
		{
			claimedBytes = 1;
			value = c;
		}
		else if ((c & 0xE0) == 0xC0)
		{
			claimedBytes = 2;
			value = c & 0x1F;
		}
		else if ((c & 0xF0) == 0xE0)
		{
			claimedBytes = 3;
			value = c & 0x0F;
		}
		else if ((c & 0xF8) == 0xF0)
		{
			claimedBytes = 4;
			value = c & 0x7;
		}
		else
		{
			/* Кодов длиннее 4 байт нет в Unicode */

			return false;
		}

		bytesInChar = 1;
	}

	return true;
}

/*
static int integer_pow(int base, int exp)
{
	int result = 1;

	while (exp)
	{
		if (exp & 1)
		{
			result *= base;
		}
		
		exp >>= 1;
		base *= base;
	}

	return result;
}
*/

/* Алгоритм похожий на java.lang.String hashCode
 * http://www.cogs.susx.ac.uk/courses/dats/notes/html/node114.html */

int string_hash(const std::string& s)
{
	int hash = 0;

	for (int i = 0; i < s.length(); ++i)
	{
		hash = 31 * hash + s[i];
	}

	return hash;
}


