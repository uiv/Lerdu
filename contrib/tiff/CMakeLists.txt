INCLUDE(inc.cmake)

#add_definitions()

include_directories(AFTER
../../inc/include
)


IF(OS_CONTRIB_TIFF_SRC)
	ADD_LIBRARY(tiff_static STATIC ${OS_CONTRIB_TIFF_SRC})
	SET_TARGET_PROPERTIES(tiff_static PROPERTIES OUTPUT_NAME tiff)
	ADD_LIBRARY(tiff SHARED ${OS_CONTRIB_TIFF_SRC})
	SET_TARGET_PROPERTIES(tiff PROPERTIES OUTPUT_NAME tiff)
	SET_TARGET_PROPERTIES(tiff PROPERTIES CLEAN_DIRECT_OUTPUT 1)
	SET_TARGET_PROPERTIES(tiff PROPERTIES SOVERSION ${VERSION})
ENDIF()
