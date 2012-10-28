####################################################################
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_CROSSCOMPILING 1)
#/home/samuel/os/avr8-toolchain/bin/
#/home/samuel/F/avr8-toolchain/bin/avr-ar  C:/Program Files/Atmel/AVR Tools/AVR Toolchain/bin/avr-
#C:/Program Files/Atmel/AVR Tools/AVR Toolchain/bin/
SET(CMAKE_AR_COMPILER "/home/samuel/os/avr8-toolchain/bin/avr-ar")
SET(CMAKE_AR_COMPILER_ID GUN)#CLANG
SET(CMAKE_AR_COMPILER_WORKS TRUE)

SET(CMAKE_C_COMPILER "/home/samuel/os/avr8-toolchain/bin/avr-gcc")
SET(CMAKE_C_COMPILER_ID_RUN TRUE)
SET(CMAKE_C_COMPILER_ID GUN)#CLANG
SET(CMAKE_C_COMPILER_WORKS TRUE)
SET(CMAKE_C_COMPILER_FORCED TRUE)

SET(CMAKE_CXX_COMPILER "/home/samuel/os/avr8-toolchain/bin/avr-c++")
SET(CMAKE_CXX_COMPILER_COMPILER_ID_RUN TRUE)
SET(CMAKE_CXX_COMPILER_ID GUN)
SET(CMAKE_CXX_COMPILER_WORKS TRUE)
SET(CMAKE_CXX_COMPILER_FORCED TRUE)

SET(CMAKE_ASM_COMPILER "/home/samuel/os/avr8-toolchain/bin/avr-gcc")
SET(CMAKE_ASM_COMPILER_COMPILER_ID_RUN TRUE)
SET(CMAKE_ASM_COMPILER_ID GUN)
SET(CMAKE_ASM_COMPILER_WORKS TRUE)
SET(CMAKE_ASM_COMPILER_FORCED TRUE)

SET(CMAKE_LINKER_COMPILER "/home/samuel/os/avr8-toolchain/bin/avr-ld")
SET(CMAKE_LINKER_COMPILER_COMPILER_ID_RUN TRUE)
SET(CMAKE_LINKER_COMPILER_ID GUN)
SET(CMAKE_LINKER_COMPILER_WORKS TRUE)
SET(CMAKE_LINKER_COMPILER_FORCED TRUE)

SET(CMAKE_NM_COMPILER "/home/samuel/os/avr8-toolchain/bin/avr-nm")
SET(CMAKE_NM_COMPILER_ID GUN)
SET(CMAKE_NM_COMPILER_WORKS TRUE)

SET(CMAKE_OBJCOPY_COMPILER "/home/samuel/os/avr8-toolchain/bin/avr-objcopy")
SET(CMAKE_OBJCOPY_COMPILER_ID GUN)
SET(CMAKE_OBJCOPY_COMPILER_WORKS TRUE)

SET(CMAKE_OBJDUMP_COMPILER "/home/samuel/os/avr8-toolchain/bin/avr-objdump")
SET(CMAKE_OBJDUMP_COMPILER_ID GUN)
SET(CMAKE_OBJDUMP_COMPILER_WORKS TRUE)

SET(CMAKE_STRIP_COMPILER "/home/samuel/os/avr8-toolchain/bin/avr-strip")
SET(CMAKE_STRIP_COMPILER_ID GUN)
SET(CMAKE_STRIP_COMPILER_WORKS TRUE)


SET(CMAKE_MAKE_PROGRAM "make")

enable_language(ASM)

########################################################################
SET(CMAKE_AR_FLAGS "rc")
SET(MMCU "-mmcu=atmega128")
#-DF_CPU=16000000UL ${MMCU}  -lprintf_flt -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fno-strict-aliasing -Wstrict-prototypes -fno-unit-at-a-time -Wundef -Wunreachable-code -Wsign-compare -Wa,-adhlns=$(<:%.c=%.lst),-gstabs -gdwarf-2 -fno-inline-small-functions -finline-limit=3 -fno-split-wide-types -fno-tree-scev-cprop -ffunction-sections -fdata-sections -ffast-math -mrelax -fno-common -ffreestanding -mcall-prologues -MD -MP -MF -lm
SET(CMAKE_C_FLAGS   "-O2 -Wall -nostdinc -nostdlib")
# -Wa,-adhlns=$(<:%.S=%.lst)
SET(CMAKE_ASM_FLAGS "${MMCU}")
SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
# -Wl,--allow-multiple-definition   -Wl,-u,vfprintf  -Wl,--relax
#SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--entry=__start -Wl,-Map=ker.map,--cref")

#add_definitions(-D__AVR_ATmega128__)
#remove_definitions()

#SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
#SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)

SET(CMAKE_VERBOSE_MAKEFILE ON)

####################################################################

cmake_policy(VERSION 2.4)
cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

project(Lerdu)

########################################################################
#SET(uARCH "${Lerdu_SOURCE_DIR}/sys/arch/i386")
#SET(uBOOT "${Lerdu_SOURCE_DIR}/sys/arch/i386/stand")
#AFTER
include_directories(
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/include/std
${CMAKE_CURRENT_SOURCE_DIR}/include/stl
${CMAKE_CURRENT_SOURCE_DIR}/arch/x86/include
${CMAKE_CURRENT_SOURCE_DIR}/bsp/hal/x86/include/
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/lib
${CMAKE_CURRENT_SOURCE_DIR}/fs
${CMAKE_CURRENT_SOURCE_DIR}/net

${CMAKE_CURRENT_SOURCE_DIR}/include/sys



${CMAKE_CURRENT_SOURCE_DIR}
)

#add_subdirectory(contrib)
add_subdirectory(arch)
add_subdirectory(atom)
add_subdirectory(net)
add_subdirectory(fs)
add_subdirectory(lib)
add_subdirectory(bsp)
#add_subdirectory(com)

#add_subdirectory(netutils)
#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/stdio)
#add_executable(helloDemo demo.cxx demo_b.cxx)
#target_link_libraries (helloDemo Hello)
#add_library (Hello hello.cxx)
#参考生成动态库
#add_library(zlib SHARED ${ZLIB_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
#add_library(zlibstatic STATIC ${ZLIB_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
#set_target_properties(zlib PROPERTIES DEFINE_SYMBOL ZLIB_DLL)
#set_target_properties(zlib PROPERTIES SOVERSION 1)
#if(UNIX)
#    # On unix-like platforms the library is almost always called libz
#   set_target_properties(zlib zlibstatic PROPERTIES OUTPUT_NAME z)
#   set_target_properties(zlib PROPERTIES LINK_FLAGS "-Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/zlib.map")
#elseif(BUILD_SHARED_LIBS AND WIN32)
#    # Creates zlib1.dll when building shared library version
#    set_target_properties(zlib PROPERTIES SUFFIX "1.dll")
#endif()
