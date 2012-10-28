cmake_minimum_required(VERSION 2.8)

PROJECT(librsvg)

SET(VERSION "0.1.07")

#SET(CMAKE_C_FLAGS   "-O2 -Wcpp")

#add_definitions(-DHAVE_BCOPY)

include_directories(AFTER
../../inc/include/gdk-pixbuf-2.0
../../inc/include/pango-1.0
../libxml/include
../pango
../cairo/src
../glib/gmodule
../glib/glib
../glib
)

SET(CURPATH "")
INCLUDE(inc.cmake)

IF(OS_CONTRIB_LIBRSVG_SRC)
	ADD_LIBRARY(librsvg SHARED ${OS_CONTRIB_LIBRSVG_SRC})
	SET_TARGET_PROPERTIES(librsvg PROPERTIES OUTPUT_NAME rsvg)
	SET_TARGET_PROPERTIES(librsvg PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	SET_TARGET_PROPERTIES(librsvg PROPERTIES SOVERSION ${VERSION})
	#ADD_LIBRARY(librsvg_static STATIC ${OS_CONTRIB_LIBRSVG_SRC})
	#SET_TARGET_PROPERTIES(librsvg_static PROPERTIES OUTPUT_NAME rsvg)
ENDIF()
