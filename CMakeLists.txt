# Автор: Пронин И.С. <luminex@bk.ru>
# 2013 г.

cmake_minimum_required (VERSION 2.6)
project (u)
file (GLOB SOURCES *.cxx)

#set (COMPILE_FLAGS "-O4 --profile-generate")
#set (COMPILE_FLAGS "-O4 --profile-use")
#set (LINK_FLAGS_RELEASE "--profile-generate")
#set (LINK_FLAGS "--profile-use")
set (COMPILE_FLAGS "-O0")

set_source_files_properties(${SOURCES} PROPERTIES
	COMPILE_FLAGS "${COMPILE_FLAGS} -std=gnu++0x -Wall")

add_executable (u ${SOURCES})
target_link_libraries (u pthread)

set_target_properties (u PROPERTIES
	LINK_FLAGS "${LINK_FLAGS}")

target_link_libraries (u gcov)

