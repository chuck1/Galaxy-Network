CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)

PROJECT(chat)

ADD_EXECUTABLE(chat main.cpp)


#ADD_DEFINITIONS("-DBOOST_ALL_DYN_LINK")
#
##SET(Boost_USE_STATIC_LIBS ON)
#
#SET(Boost_NO_SYSTEM_PATHS ON)
#SET(BOOST_ROOT /scratch/usr)
#
#
#FIND_PACKAGE(Boost 1.55 COMPONENTS serialization system thread REQUIRED)
#
#IF(Boost_FOUND)
#ELSE()
#	MESSAGE(STATUS "boost not found")
#ENDIF()
#
#
#
#MESSAGE(STATUS "boost: ${Boost_SERIALIZATION_LIBRARY}")
#MESSAGE(STATUS "boost: ${Boost_SYSTEM_LIBRARY}")
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
	pthread
	rt
	)

TARGET_LINK_LIBRARIES(chat Galaxy-Network ${libs})

