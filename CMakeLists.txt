cmake_minimum_required (VERSION 3.8)
project("vm")

set(COMPILER_OPTS_GCC "")
set(COMPILER_OPTS_MSVC "/D_CRT_SECURE_NO_WARNINGS")

# Figure out if the machine is little or big endian
include(TestBigEndian)
TEST_BIG_ENDIAN(OB_IS_BIG_ENDIAN)
if(${OB_IS_BIG_ENDIAN})
    set(COMPILER_OPTS_GCC "${COMPILER_OPTS_GCC} -DOBERON_BIG_ENDIAN")
    set(COMPILER_OPTS_MSVC "${COMPILER_OPTS_MSVC} /DOBERON_BIG_ENDIAN")
else()
    set(COMPILER_OPTS_GCC "${COMPILER_OPTS_GCC} -DOBERON_LITTLE_ENDIAN")
    set(COMPILER_OPTS_MSVC "${COMPILER_OPTS_MSVC} /DOBERON_LITTLE_ENDIAN")
endif()

# Set options for debug- and release builds
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(COMPILER_OPTS_GCC "${COMPILER_OPTS_GCC} -D_DEBUG")
    set(COMPILER_OPTS_MSVC "${COMPILER_OPTS_MSVC} /D_DEBUG")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    set(COMPILER_OPTS_GCC "${COMPILER_OPTS_GCC} -O3 -DNDEBUG")
    set(COMPILER_OPTS_MSVC "${COMPILER_OPTS_MSVC} /O2 /Ot /DNDEBUG")
endif()

# Set options to editor
if(WIN32)
    ADD_DEFINITIONS("${COMPILER_OPTS_MSVC}")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_OPTS_GCC}")
endif()

# Virtual Machine and Pipeline Builder
set(VM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_library(vm 
    "src/vm_string.c" 
    "src/vm_bytestream.c" 
    "src/vm_message.c" 
    "src/vm_debug.c" 
    "src/vm_hash.c"
    "src/vm_string_pool.c"

    "src/interpreter/stack.c" 
    "src/interpreter/thread.c" 
    "src/interpreter/process.c"
    "src/interpreter/debug.c" 
    "src/interpreter/locals.c" 
    "src/interpreter/lists.c"  

    "src/vmp/vmp.c" 
    "src/vmp/vmp_instr.c" 
    "src/vmp/vmp_types.c"
    "src/vmp/vmp_messages.c"
    "src/vmp/vmp_string_pool.c"
    "src/vmp/vmp_list_args.c" 
    "src/vmp/vmp_list_packages.c"
    "src/vmp/vmp_list_types.c" 
    
    "src/vmp/vmp_list_returns.c" 
    "src/vmp/vmp_list_locals.c" 
    "src/vmp/vmp_list_locals.h" 
    "src/vmp/vmp_list_markers.c" 
    
    "src/vmp/vmp_list_globals.c"
    "src/vmp/vmp_list_imports.c"
    "src/vmp/vmp_lists.c"
    
    "src/vmp/const.c"
    "src/vmp/const_add.c"
    "src/vmp/const_sub.c"
    "src/vmp/const_mult.c"
    "src/vmp/const_div.c")

# Compiler for ZPP source code
include_directories("${VM_INCLUDE_DIRS}")
add_library(zpp 
    "src/zpp/syntax_tree.c" 
    "src/zpp/lexer.c" 
    "src/zpp/symbols.c" 
    "src/zpp/lists.c" 
    "src/zpp/compiler.c" 
    "src/zpp/source_code.c" 
    "src/zpp/messages.c"
    "src/zpp/optimizations.c"
)

# Tests
set(CMAKE_CXX_STANDARD 17)
include_directories("${VM_INCLUDE_DIRS}")
add_executable(test "test/main.cpp" "test/test.hpp" "test/utils.hpp" "test/suite_vmp.cpp" "test/suite_zpp.cpp")
target_link_libraries(test zpp vm)