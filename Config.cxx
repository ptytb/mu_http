#include "Config.hpp"

#include <fstream>

int Config::readConfig(std::string path, std::string locale)
{
	std::ifstream cf(path.c_str(), std::ifstream::in);
	cf.imbue(std::locale(locale.c_str()));

	if (!cf.good()) {
		return CONFIG_NOT_FOUND;
	}

	for (;;) {
		std::string line;
		getline(cf, line);

		if (cf.eof()) {
			break;
		}

		if (!line.length()) {
			continue;
		}

		size_t rem = line.find_first_of("#");

		if (rem == 0) {
			continue;
		}

		size_t delim = line.find_first_of("=");
		
		if (delim == std::string::npos) {
			return CONFIG_SYNTAX_ERROR;
		}

		config[line.substr(0, delim)] = line.substr(delim + 1);
	}

	cf.close();

	return CONFIG_LOADED;
}


