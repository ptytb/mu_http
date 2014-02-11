/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#include "DirReader.hpp"

#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stddef.h>
#include <sys/inotify.h>
#include <set>

#include "ustring.hpp"
#include "Log.hpp"

DirReader::DirReader() :
	fd(inotify_init1(IN_NONBLOCK))
{
	if (fd < 0)
	{
		throw std::exception();
	}
}

DirReader::~DirReader()
{
	close(fd);
}

void DirReader::cache(std::string& root)
{
	this->root = root;
	std::string rt("/");
	int hash = string_hash(rt);
	bool dummy;

	addMeta(root, rt, hash, dummy);
	addWatch(root, hash);
	scan(rt, rt, true, hash);
}

void DirReader::scan(std::string path, std::string name, bool recursive,
		int parentHash)
{
	std::string fullPath(root);
	fullPath.append(path);

	DIR *dir = opendir(fullPath.c_str());

	if (!dir || chdir(fullPath.c_str()))
	{
		return;
	}
	
	dirent entry, *result;

	for (;;)
	{
		int re = readdir_r(dir, &entry, &result);
		
		if (re || !result)
		{
			break;
		}

		std::string name(result->d_name);

		if (name == "." || name == "..")
		{
			continue;
		}

		std::string pathName(path);
		if (pathName != "/")
		{
			pathName.append("/");
		}
		pathName.append(name);

		std::string fullPathName(root);
		fullPathName.append(pathName);

		int hash = string_hash(pathName);
		bool isDir; /* (result->d_type == DT_DIR) не работает на XFS */
		
		if (!addMeta(fullPathName, name, hash, isDir))
		{
			continue;
		}
		
		pedigree.insert(std::make_pair(parentHash, hash));

		if (isDir && recursive)
		{
			addWatch(fullPathName, hash);
			scan(pathName, name, recursive, hash);
		}
	}

	closedir(dir);
}

void DirReader::addWatch(std::string fullPath, int hash)
{
	int wd = inotify_add_watch(fd, fullPath.c_str(), EVENTS);
	notify[wd] = hash;
}

bool DirReader::addMeta(std::string fullPath, std::string name, int hash,
		bool& isDir)
{
	struct stat fs;

	int r = lstat(fullPath.c_str(), &fs);

	if (r || S_ISLNK(fs.st_mode))
	{
		return false;
	}

	isDir = S_ISDIR(fs.st_mode);

	meta.insert(std::make_pair(
				hash, FileInfo(name,
					isDir,
					fs.st_mtime,
					fs.st_size)) );
	return true;
}

std::string DirReader::fold(std::string uri)
{
	size_t p;

	// http://adsf/sadf.hgut/../text.doc
	// ---------------------^   ^------->

	while ((p = uri.find("/../") != std::string::npos))
	{
		std::string base = uri.substr(0, p);
		FileInfo& fi = getMeta(base);
	}
}

std::string DirReader::extractParent(std::string uri)
{
	size_t sl = uri.rfind('/');
	return (sl == std::string::npos || sl == 0)
		? "/"
		: uri.substr(0, sl);
}

void DirReader::getDir(std::string& path, std::map<int, int>::iterator& begin,
		std::map<int, int>::iterator& end)
{
	int hash = string_hash(path);
	begin = pedigree.lower_bound(hash);
	end = pedigree.upper_bound(hash);
}

FileInfo& DirReader::getMeta(int hash)
{
	std::map<int, FileInfo>::iterator i(meta.find(hash));

	if (i == meta.end())
	{
		throw std::exception();
	}

	return i->second;
}

FileInfo& DirReader::getMeta(std::string& path)
{
	return getMeta(string_hash(path));
}

FileInfo& DirReader::getMetaByWatchDescriptor(int wd)
{
	return getMeta(notify[wd]);
}

void DirReader::update(int wd, uint32_t mask)
{
	FileInfo& fi = getMetaByWatchDescriptor(wd);

	log("Обновление директории: %s", fi.name.c_str());
	
}


