#cmake_minimum_required(VERSION 2.8)

#PROJECT(DirectFB)

#SET(VERSION "0.1.07")

#SET(CMAKE_C_FLAGS   "-O2 -Wcpp")

include_directories(AFTER
./gfxdrivers/davinci
./gfxdrivers/davinci/kernel-module/include
./proxy/requestor
./proxy/dispatcher
./gfxdrivers/nsc/include
../tslib/src
../freetype/include
../jpeg
../libpng
../../inc/include/libpng15
./wm
./
./include
./systems
./lib
./src
)

SET(CURPATH "")
INCLUDE(inc.cmake)

IF(OS_CONTRIB_DIRECTFB_SRC)
	ADD_LIBRARY(DirectFB SHARED ${OS_CONTRIB_DIRECTFB_SRC})
	SET_TARGET_PROPERTIES(DirectFB PROPERTIES OUTPUT_NAME dfb)
	SET_TARGET_PROPERTIES(DirectFB PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	SET_TARGET_PROPERTIES(DirectFB PROPERTIES SOVERSION ${VERSION})
	#ADD_LIBRARY(zlib_static STATIC ${OS_CONTRIB_DIRECTFB_SRC})
	#SET_TARGET_PROPERTIES(zlib_static PROPERTIES OUTPUT_NAME dfb)
ENDIF()
