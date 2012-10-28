# Install script for directory: /home/samuel/build/1.08/contrib

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/samuel/build/1.08/contrib/zlib/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/v8/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/bzip/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/gzip/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/expat/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/jpeg/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/libmng/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/libogg/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/libpng/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/libxml/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/pixman/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/tiff/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/tslib/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/thttpd/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/sqlite/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/php/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/libvpx/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/libiconv/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/cairo/cmake_install.cmake")
  INCLUDE("/home/samuel/build/1.08/contrib/freetype/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

