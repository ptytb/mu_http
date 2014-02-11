/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#pragma once

#include <string>
#include <map>

std::string stime(std::string fmt, const time_t *t);
std::string uriDecode(std::string encUri);
std::string uriEncode(std::string uri);
void parseArgs(std::string queryString,
		std::map<std::string, std::string> &query);
bool utf8Check(std::string &s);
int string_hash(const std::string& s);

