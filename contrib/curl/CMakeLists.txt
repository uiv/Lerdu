cmake_minimum_required(VERSION 2.8)

PROJECT(curl)

SET(VERSION "0.1.07")

#SET(CMAKE_C_FLAGS   "-O2 -Wcpp")

include_directories(AFTER
./include
)

SET(CURPATH "")
INCLUDE(inc.cmake)

IF(OS_CONTRIB_CURL_SRC)
	ADD_LIBRARY(curl SHARED ${OS_CONTRIB_CURL_SRC})
	SET_TARGET_PROPERTIES(curl PROPERTIES OUTPUT_NAME curl)
	SET_TARGET_PROPERTIES(curl PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	SET_TARGET_PROPERTIES(curl PROPERTIES SOVERSION ${VERSION})
	#ADD_LIBRARY(curl_static STATIC ${OS_CONTRIB_CURL_SRC})
	#SET_TARGET_PROPERTIES(curl_static PROPERTIES OUTPUT_NAME curl)
ENDIF()
