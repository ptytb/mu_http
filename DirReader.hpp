/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#pragma once

#include <string>
#include <list>
#include <map>

#define EVENTS IN_ATTRIB | IN_CLOSE_WRITE | IN_CREATE | IN_DELETE\
| IN_DELETE_SELF | IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO

class FileInfo
{
	public:
		FileInfo(std::string name, bool isDir, time_t modified,
				size_t length) :
			name(name),
			isDir(isDir),
			modified(modified),
			length(length)
		{}

		std::string name;
		bool isDir;
		time_t modified;
		size_t length;
};

class DirReader
{
	public:
		int fd;
		std::string root;

		DirReader();
		~DirReader();

		void cache(std::string& root);
		void getDir(std::string& path, std::map<int, int>::iterator& begin,
				std::map<int, int>::iterator& end);
		FileInfo& getMeta(int hash);
		FileInfo& getMeta(std::string& path);
		FileInfo& getMetaByWatchDescriptor(int wd);
		void update(int wd, uint32_t mask);

		static std::string extractParent(std::string uri);

	protected:

		std::map<int, FileInfo> meta;
		std::multimap<int, int> pedigree; /* parent hash => child hash */
		std::map<int, int> notify; /* fd => hash */

		void scan(std::string path, std::string name, bool recursive,
				int parentHash);
		void addWatch(std::string fullPath, int hash);
		bool addMeta(std::string fullPath, std::string name,
				int hash, bool& isDir);
		std::string fold(std::string uri);
};


