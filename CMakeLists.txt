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
    "src/vm_string.h" "src/vm_string.c"
    "src/vm_bytestream.h" "src/vm_bytestream.c"
    "src/vm_message.h" "src/vm_message.c"

    "src/interpreter/vmi_ops.h" "src/interpreter/vmi_stack.h"  "src/interpreter/vmi_stack.c" "src/interpreter/vmi_thread.h" 
    "src/interpreter/vmi_thread.c" "src/interpreter/vmi_process.h" "src/interpreter/vmi_process.c" "src/interpreter/vmi_config.h" 
    "src/interpreter/vmi_debug.h" "src/interpreter/vmi_debug.c" 
     "src/vmc/vmc_linker_marker.c" "src/vmc/vmc_linker_messages.h" "src/vmc/vmc_linker_messages.c" "src/vmc/vmc_debug.h" "src/vmc/vmc_debug.c" 
     "src/vmp/vmp_list_packages.h" "src/vmc/lists/packages_list.c" 
     
     "src/vmp/vmp.h" "src/vmp/vmp_types.h" "src/vmp/vmp_instr.h" "src/vmp/vmp_instr.c" 
     "src/vmp/vmp.c" "src/vmp/vmp_list_args.h" "src/vmp/vmp_list_args.c" "src/vmp/vmp_types.c" 
     "src/vmp/vmp_list_packages.c" "src/vmp/vmp_list_packages.h" "src/vmp/vmp_list_types.h" 
     "src/vmp/vmp_list_types.c" "src/vmp/vmp_list_funcs.c" "src/vmp/vmp_list_funcs.h"  "src/vmp/vmp_list_returns.c" 
     "src/vmp/vmp_list_returns.h" "src/vmp/vmp_messages.h" "src/vmp/vmp_messages.c" "src/vmp/vmp_list_locals.c" 
     "src/vmp/vmp_list_locals.h" "src/vmp/vmp_list_markers.c" "src/vmp/vmp_list_markers.h" "src/interpreter/vmi_list_packages.h" "src/interpreter/vmi_list_packages.c" "src/interpreter/vmi_list_functions.c" "src/interpreter/vmi_list_functions.h" "src/vmp/vmp_list_inherits_from.c" "src/vmp/vmp_list_inherits_from.h" "src/vmp/vmp_list_inherited_by.c" "src/vmp/vmp_list_inherited_by.h")


# Tests
set(CMAKE_CXX_STANDARD 17)
include_directories("${VM_INCLUDE_DIRS}")
add_executable(test "test/main.cpp" "test/test.hpp" "test/utils.hpp" "test/suite_vmp.cpp")
target_link_libraries(test vm)