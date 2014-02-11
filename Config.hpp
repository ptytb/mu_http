#pragma once

#include <string>
#include <map>

#define CONFIG_NOT_FOUND -1
#define CONFIG_SYNTAX_ERROR -2
#define CONFIG_LOADED 0

class Config
{
	public:
		int readConfig(std::string path, std::string locale);
		
		std::string &operator[](const std::string &name) {
			return config[name];
		}

		std::map<std::string, std::string>::iterator begin() {
			return config.begin();
		}

		std::map<std::string, std::string>::iterator end() {
			return config.end();
		}

	private:
		std::map<std::string, std::string> config;

};


