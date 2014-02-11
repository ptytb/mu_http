#pragma once

#include "Page.hpp"
#include "Config.hpp"
#include "Request.hpp"

#include <string>

class PageFactory
{
	public:
		static Page* browse(std::string path);
		static Page* error(int code);
		static Page* post(Request& request);
};


