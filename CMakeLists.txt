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

# Lexer
set(VM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_library(vm "src/vm_config.h"
     "src/compiler/vm_lexer.h" "src/compiler/vm_lexer.c"
    "src/compiler/vm_compiler.h" "src/vm_bytestream.h" "src/vm_bytestream.c" "src/compiler/vm_compiler.c" 
      "src/vm_string.h" "src/vm_string.c"
    "src/interpreter/vmi_ops.h" "src/interpreter/vmi_stack.h"  "src/interpreter/vmi_stack.c" "src/interpreter/vmi_thread.h" 
    "src/interpreter/vmi_thread.c" "src/interpreter/vmi_process.h" "src/interpreter/vmi_process.c"
    "src/compiler/vm_error.h"  "src/compiler/vm_lexer_math.h" "src/compiler/vm_lexer_logic.h")

# Tests
set(CMAKE_CXX_STANDARD 17)
include_directories("${VM_INCLUDE_DIRS}")
add_executable(test "test/main.cpp" "test/test.hpp"  "test/suite_vm.cpp")
target_link_libraries(test vm)