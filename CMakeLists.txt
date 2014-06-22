CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)

INCLUDE("$ENV{HOME}/.config.cmake")

PROJECT(Galaxy-Network)


INCLUDE_DIRECTORIES("../Galaxy-Standard/include")
LINK_DIRECTORIES("../Galaxy-Standard/Debug")


FIND_PACKAGE(CMakeHelper)

FIND_PACKAGE(Boost 1.55 COMPONENTS system thread REQUIRED)

SET(libs
	Galaxy-Network
	Galaxy-Standard
	${Boost_SYSTEM_LIBRARY}
	pthread
	rt
	)



cmh_static_library()

MESSAGE(STATUS "cxx flags: ${CMAKE_CXX_FLAGS}")

ADD_SUBDIRECTORY(test)

