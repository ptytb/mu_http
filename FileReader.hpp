/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#pragma once

#include <fstream>

class FileReader
{
	public:
		FileReader(std::string name);
		size_t read(void *buf, int len);

	private:
		std::ifstream ifs;
};


