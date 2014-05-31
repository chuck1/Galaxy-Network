CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)

PROJECT(chat_server)

ADD_EXECUTABLE(chat_server main.cpp)


#ADD_DEFINITIONS("-DBOOST_ALL_DYN_LINK")
#
SET(Boost_USE_STATIC_LIBS ON)
#
SET(Boost_NO_SYSTEM_PATHS ON)
SET(BOOST_ROOT /scratch/usr)
#
#
FIND_PACKAGE(Boost 1.55 COMPONENTS system thread REQUIRED)
#
#IF(Boost_FOUND)
#ELSE()
#	MESSAGE(STATUS "boost not found")
#ENDIF()
#
#
#
#MESSAGE(STATUS "boost: ${Boost_SERIALIZATION_LIBRARY}")
MESSAGE(STATUS "boost: ${Boost_SYSTEM_LIBRARY}")
#MESSAGE(STATUS "boost: ${Boost_THREAD_LIBRARY}")
#
#
#SET(libs
#	${Boost_SERIALIZATION_LIBRARY}
#	${Boost_SYSTEM_LIBRARY}
#	${Boost_THREAD_LIBRARY}
#	pthread
#	rt
#	)
#
#SET(libs ${libs} ${libs})

SET(libs
	${Boost_SYSTEM_LIBRARY}
	pthread
	rt
	)

TARGET_LINK_LIBRARIES(chat_server Galaxy-Network ${libs})

