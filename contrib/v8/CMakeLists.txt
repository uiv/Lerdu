####################################################################
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_CROSSCOMPILING 1)



########################################################################
SET(CMAKE_AR_FLAGS "rc")
SET(MMCU "-mmcu=atmega128")
#-DF_CPU=16000000UL ${MMCU}  -lprintf_flt -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fno-strict-aliasing -Wstrict-prototypes -fno-unit-at-a-time -Wundef -Wunreachable-code -Wsign-compare -Wa,-adhlns=$(<:%.c=%.lst),-gstabs -gdwarf-2 -fno-inline-small-functions -finline-limit=3 -fno-split-wide-types -fno-tree-scev-cprop -ffunction-sections -fdata-sections -ffast-math -mrelax -fno-common -ffreestanding -mcall-prologues -MD -MP -MF -lm
#-nostdinc -nostdlib
SET(CMAKE_C_FLAGS   "-O2 -Wall -Wall -W -Wundef -Wpointer-arith -Wcast-align -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs")
# -Wa,-adhlns=$(<:%.S=%.lst)
#SET(CMAKE_ASM_FLAGS "${MMCU}")
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
#	

include_directories(AFTER
/home/samuel/build/1.07/contrib/v8/include
/home/samuel/build/1.07/contrib/v8/src
)

SET(OS_CONTRIB_V8
${Lerdu_SOURCE_DIR}/src/accessors.cc
${Lerdu_SOURCE_DIR}/src/counters.cc
${Lerdu_SOURCE_DIR}/src/strtod.cc
${Lerdu_SOURCE_DIR}/src/allocation.cc
${Lerdu_SOURCE_DIR}/src/full-codegen.cc
${Lerdu_SOURCE_DIR}/src/log.cc
${Lerdu_SOURCE_DIR}/src/cpu-profiler.cc
${Lerdu_SOURCE_DIR}/src/func-name-inferrer.cc
${Lerdu_SOURCE_DIR}/src/property.cc
${Lerdu_SOURCE_DIR}/src/api.cc
${Lerdu_SOURCE_DIR}/src/log-utils.cc
${Lerdu_SOURCE_DIR}/src/gdb-jit.cc
${Lerdu_SOURCE_DIR}/src/token.cc
${Lerdu_SOURCE_DIR}/src/d8.cc
${Lerdu_SOURCE_DIR}/src/d8-debug.cc
${Lerdu_SOURCE_DIR}/src/global-handles.cc
${Lerdu_SOURCE_DIR}/src/transitions.cc
${Lerdu_SOURCE_DIR}/src/mark-compact.cc
${Lerdu_SOURCE_DIR}/src/regexp-macro-assembler.cc
${Lerdu_SOURCE_DIR}/src/handles.cc
${Lerdu_SOURCE_DIR}/src/regexp-macro-assembler-irregexp.cc
${Lerdu_SOURCE_DIR}/src/type-info.cc
${Lerdu_SOURCE_DIR}/src/assembler.cc
${Lerdu_SOURCE_DIR}/src/d8-posix.cc
${Lerdu_SOURCE_DIR}/src/messages.cc
${Lerdu_SOURCE_DIR}/src/ast.cc
#${Lerdu_SOURCE_DIR}/src/d8-readline.cc
${Lerdu_SOURCE_DIR}/src/regexp-macro-assembler-tracer.cc
${Lerdu_SOURCE_DIR}/src/d8-windows.cc
${Lerdu_SOURCE_DIR}/src/heap.cc
${Lerdu_SOURCE_DIR}/src/unicode.cc
${Lerdu_SOURCE_DIR}/src/data-flow.cc
${Lerdu_SOURCE_DIR}/src/regexp-stack.cc
${Lerdu_SOURCE_DIR}/src/date.cc
${Lerdu_SOURCE_DIR}/src/heap-profiler.cc
${Lerdu_SOURCE_DIR}/src/rewriter.cc
${Lerdu_SOURCE_DIR}/src/atomicops_internals_x86_gcc.cc
${Lerdu_SOURCE_DIR}/src/mksnapshot.cc
${Lerdu_SOURCE_DIR}/src/utils.cc
${Lerdu_SOURCE_DIR}/src/hydrogen.cc
${Lerdu_SOURCE_DIR}/src/dateparser.cc
${Lerdu_SOURCE_DIR}/src/objects.cc
${Lerdu_SOURCE_DIR}/src/hydrogen-instructions.cc
${Lerdu_SOURCE_DIR}/src/objects-debug.cc
${Lerdu_SOURCE_DIR}/src/v8.cc
${Lerdu_SOURCE_DIR}/src/bignum.cc
${Lerdu_SOURCE_DIR}/src/runtime-profiler.cc
${Lerdu_SOURCE_DIR}/src/bignum-dtoa.cc
${Lerdu_SOURCE_DIR}/src/debug-agent.cc
${Lerdu_SOURCE_DIR}/src/v8conversions.cc
${Lerdu_SOURCE_DIR}/src/ic.cc
${Lerdu_SOURCE_DIR}/src/objects-printer.cc
${Lerdu_SOURCE_DIR}/src/safepoint-table.cc
${Lerdu_SOURCE_DIR}/src/debug.cc
${Lerdu_SOURCE_DIR}/src/objects-visiting.cc
${Lerdu_SOURCE_DIR}/src/v8-counters.cc
${Lerdu_SOURCE_DIR}/src/bootstrapper.cc
${Lerdu_SOURCE_DIR}/src/scanner.cc
${Lerdu_SOURCE_DIR}/src/incremental-marking.cc
${Lerdu_SOURCE_DIR}/src/scanner-character-streams.cc
${Lerdu_SOURCE_DIR}/src/v8dll-main.cc
${Lerdu_SOURCE_DIR}/src/builtins.cc
${Lerdu_SOURCE_DIR}/src/deoptimizer.cc
${Lerdu_SOURCE_DIR}/src/once.cc
${Lerdu_SOURCE_DIR}/src/inspector.cc
${Lerdu_SOURCE_DIR}/src/optimizing-compiler-thread.cc
${Lerdu_SOURCE_DIR}/src/cached-powers.cc
${Lerdu_SOURCE_DIR}/src/disassembler.cc
${Lerdu_SOURCE_DIR}/src/scopeinfo.cc
${Lerdu_SOURCE_DIR}/src/interface.cc
${Lerdu_SOURCE_DIR}/src/parser.cc
#${Lerdu_SOURCE_DIR}/src/v8preparserdll-main.cc
${Lerdu_SOURCE_DIR}/src/diy-fp.cc
${Lerdu_SOURCE_DIR}/src/scopes.cc
${Lerdu_SOURCE_DIR}/src/v8threads.cc
${Lerdu_SOURCE_DIR}/src/interpreter-irregexp.cc
${Lerdu_SOURCE_DIR}/src/checks.cc
${Lerdu_SOURCE_DIR}/src/serialize.cc
${Lerdu_SOURCE_DIR}/src/v8utils.cc
${Lerdu_SOURCE_DIR}/src/dtoa.cc
${Lerdu_SOURCE_DIR}/src/isolate.cc
${Lerdu_SOURCE_DIR}/src/circular-queue.cc
${Lerdu_SOURCE_DIR}/src/variables.cc
${Lerdu_SOURCE_DIR}/src/elements.cc
${Lerdu_SOURCE_DIR}/src/version.cc
${Lerdu_SOURCE_DIR}/src/codegen.cc
${Lerdu_SOURCE_DIR}/src/elements-kind.cc
${Lerdu_SOURCE_DIR}/src/snapshot-common.cc
${Lerdu_SOURCE_DIR}/src/jsregexp.cc
${Lerdu_SOURCE_DIR}/src/snapshot-empty.cc
${Lerdu_SOURCE_DIR}/src/execution.cc
${Lerdu_SOURCE_DIR}/src/code-stubs.cc
${Lerdu_SOURCE_DIR}/src/spaces.cc
${Lerdu_SOURCE_DIR}/src/factory.cc
${Lerdu_SOURCE_DIR}/src/compilation-cache.cc
${Lerdu_SOURCE_DIR}/src/lithium-allocator.cc
${Lerdu_SOURCE_DIR}/src/fast-dtoa.cc
${Lerdu_SOURCE_DIR}/src/zone.cc
${Lerdu_SOURCE_DIR}/src/compiler.cc
${Lerdu_SOURCE_DIR}/src/preparse-data.cc
${Lerdu_SOURCE_DIR}/src/store-buffer.cc
${Lerdu_SOURCE_DIR}/src/fixed-dtoa.cc
${Lerdu_SOURCE_DIR}/src/lithium.cc
${Lerdu_SOURCE_DIR}/src/contexts.cc
${Lerdu_SOURCE_DIR}/src/liveedit.cc
${Lerdu_SOURCE_DIR}/src/preparser-api.cc
${Lerdu_SOURCE_DIR}/src/flags.cc
${Lerdu_SOURCE_DIR}/src/preparser.cc
${Lerdu_SOURCE_DIR}/src/string-search.cc
${Lerdu_SOURCE_DIR}/src/conversions.cc
${Lerdu_SOURCE_DIR}/src/frames.cc
${Lerdu_SOURCE_DIR}/src/liveobjectlist.cc
${Lerdu_SOURCE_DIR}/src/prettyprinter.cc
${Lerdu_SOURCE_DIR}/src/string-stream.cc

${Lerdu_SOURCE_DIR}/src/profile-generator.cc
#${Lerdu_SOURCE_DIR}/src/platform-openbsd.cc
#${Lerdu_SOURCE_DIR}/src/platform-posix.cc
#${Lerdu_SOURCE_DIR}/src/platform-solaris.cc
#${Lerdu_SOURCE_DIR}/src/platform-win32.cc
${Lerdu_SOURCE_DIR}/src/platform-linux.cc
#${Lerdu_SOURCE_DIR}/src/platform-freebsd.cc
#${Lerdu_SOURCE_DIR}/src/platform-cygwin.cc
#${Lerdu_SOURCE_DIR}/src/platform-macos.cc
#${Lerdu_SOURCE_DIR}/src/platform-nullos.cc    


#${Lerdu_SOURCE_DIR}/src/extensions/externalize-string-extension.cc
#${Lerdu_SOURCE_DIR}/src/extensions/gc-extension.cc
#${Lerdu_SOURCE_DIR}/src/extensions/statistics-extension.cc


${Lerdu_SOURCE_DIR}/src/x64/assembler-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/deoptimizer-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/ic-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/lithium-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/code-stubs-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/disasm-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/lithium-codegen-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/simulator-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/frames-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/macro-assembler-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/builtins-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/cpu-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/lithium-gap-resolver-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/stub-cache-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/codegen-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/debug-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/full-codegen-x64.cc
${Lerdu_SOURCE_DIR}/src/x64/regexp-macro-assembler-x64.cc
)


add_library(v8 SHARED ${OS_CONTRIB_V8})



#target_link_libraries(testdom alphagen art json css dom hubbub nsbmp nsgif parserutils rosprite svgtiny wapcaplet nsgenjsbind ttf2f)

#add_subdirectory(netutils)
#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/stdio)
#add_executable(helloDemo demo.cxx demo_b.cxx)
#target_link_libraries (helloDemo Hello)
#add_library (Hello hello.cxx)
