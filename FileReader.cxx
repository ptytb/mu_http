/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#include "FileReader.hpp"

FileReader::FileReader(std::string name)
{
	ifs.open(name.c_str(), std::ios::binary);
}

size_t FileReader::read(void *buf, int len)
{
	return ifs.readsome((char*) buf, len);
}


