#pragma once

/** Автор: Пронин И.С. <luminex@bk.ru>
 *  2011 г.
 */

#define SERVER_VERSION "0.2"

#define SERVER_LOCALE "ru_RU.UTF-8"
#define SERVER_ENCODING "UTF-8"

#define SERVER_CONFIG_FILE "u.conf"

#define SERVER_INFO "µ Http Сервер\n\
Копирайт (c) Пронин И.С. <mimeticdrift@gmail.com>, 2013 год.\n"

#define SERVER_BACKLOG 40

#include "Config.hpp"
#include "DirReader.hpp"

extern Config config;
extern DirReader dirReader;

