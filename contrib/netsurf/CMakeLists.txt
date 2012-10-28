include_directories(AFTER
/home/samuel/build/1.07/netsurf/netsurf
/home/samuel/build/1.07/netsurf/libwapcaplet/include
/home/samuel/build/1.07/netsurf/libcss/include
/home/samuel/build/1.07/netsurf/libdom/include
/home/samuel/build/1.07/netsurf/libhubbub/include
/home/samuel/build/1.07/netsurf/libnsbmp/include
/home/samuel/build/1.07/netsurf/libnsfb/include
/home/samuel/build/1.07/netsurf/libnsgif/include
/home/samuel/build/1.07/netsurf/librosprite
/home/samuel/build/1.07/netsurf/libparserutils/include
/home/samuel/build/1.07/netsurf/libpencil/include
/home/samuel/build/1.07/netsurf/libsvgtiny/include
/home/samuel/build/1.07/netsurf/rufl/include

/home/samuel/build/1.07/netsurf/libdom/bindings
/home/samuel/build/1.07/netsurf/libcss/src
/home/samuel/build/1.07/netsurf/libdom/src
/home/samuel/build/1.07/netsurf/libdom
/home/samuel/build/1.07/netsurf/libhubbub/src
/home/samuel/build/1.07/netsurf/libparserutils/src
/home/samuel/build/1.07/netsurf/json-c
/home/samuel/build/1.07/contrib/netsurf/hubbub-0.1.2/src/tokeniser
/home/samuel/build/1.07/contrib/netsurf/libparserutils-0.1.1/src/charset


/home/samuel/build/1.07/contrib/gtk+

/home/samuel/build/1.07/contrib/cairo/src
/home/samuel/build/1.07/contrib/librsvg-2.36.3
/home/samuel/build/1.07/contrib/freetype-2.4.10/include
/home/samuel/build/1.07/contrib/gstreamer/include/gstreamer
/home/samuel/build/1.07/contrib/gstreamer/include
/home/samuel/build/1.07/contrib/libmng-1.0.10
/home/samuel/build/1.07/contrib/libxml2/include
/home/samuel/build/1.07/contrib/libxml2
/home/samuel/build/1.07/contrib/glib/glib
/home/samuel/build/1.07/contrib/glib/gmodule
/home/samuel/build/1.07/contrib/glib
/home/samuel/build/1.07/contrib/pango
/home/samuel/build/1.07/contrib/libpng-1.5.12




/home/samuel/build/1.07/contrib/mozilla-release/config/system_wrappers
/home/samuel/build/1.07/contrib/mozilla-release/js/src/config/system_wrappers_js
/home/samuel/build/1.07/contrib/mozilla-release/js
/home/samuel/build/1.07/contrib/mozilla-release

/home/samuel/build/1.07/inc/include/libpng15

/home/samuel/build/1.07/inc/include/gdk-pixbuf-2.0
/home/samuel/build/1.07/inc/include/gtk-3.0
/home/samuel/build/1.07/inc/include/atk-1.0
/home/samuel/build/1.07/inc/include/pango-1.0
)

add_subdirectory(netsurf)
add_subdirectory(alphagen)
add_subdirectory(art)
add_subdirectory(buildsystem)
add_subdirectory(json-c)
add_subdirectory(libcss)
add_subdirectory(libdom)
add_subdirectory(libhubbub)
add_subdirectory(libnsbmp)
add_subdirectory(libnsgif)
add_subdirectory(libparserutils)
add_subdirectory(librosprite)
add_subdirectory(libsvgtiny)
add_subdirectory(libwapcaplet)
add_subdirectory(nsgenjsbind)
add_subdirectory(ttf2f)
add_subdirectory(libnsfb)
#add_subdirectory(rufl)
#add_subdirectory(libpencil)
#add_subdirectory(iconv)



link_directories(
${Lerdu_SOURCE_DIR}/alphagen
${Lerdu_SOURCE_DIR}/art
${Lerdu_SOURCE_DIR}/buildsystem
${Lerdu_SOURCE_DIR}/json-c
${Lerdu_SOURCE_DIR}/libcss
${Lerdu_SOURCE_DIR}/libdom
${Lerdu_SOURCE_DIR}/libhubbub
${Lerdu_SOURCE_DIR}/libnsbmp
${Lerdu_SOURCE_DIR}/libnsgif
${Lerdu_SOURCE_DIR}/libparserutils
${Lerdu_SOURCE_DIR}/libpencil
${Lerdu_SOURCE_DIR}/librosprite
${Lerdu_SOURCE_DIR}/libsvgtiny
${Lerdu_SOURCE_DIR}/libwapcaplet
${Lerdu_SOURCE_DIR}/nsgenjsbind
${Lerdu_SOURCE_DIR}/rufl
${Lerdu_SOURCE_DIR}/ttf2f
${Lerdu_SOURCE_DIR}/netsurf
${Lerdu_SOURCE_DIR}/libnsfb
${Lerdu_SOURCE_DIR}/iconv
)
#add_executable(testdom ${Lerdu_SOURCE_DIR}/libdom/examples/dom-structure-dump.c)

#target_link_libraries(testdom alphagen art json css dom hubbub nsbmp nsgif parserutils rosprite svgtiny wapcaplet nsgenjsbind ttf2f)
