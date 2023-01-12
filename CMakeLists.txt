cmake_minimum_required (VERSION 3.8)
project("vm")

set(COMPILER_OPTS_GCC "-DARLANG_MEMORY_TRACKING")
set(COMPILER_OPTS_MSVC "/D_CRT_SECURE_NO_WARNINGS /DARLANG_MEMORY_TRACKING")

# Figure out if the machine is little or big endian
include(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(${IS_BIG_ENDIAN})
    set(COMPILER_OPTS_GCC "${COMPILER_OPTS_GCC} -DARLANG_BIG_ENDIAN")
    set(COMPILER_OPTS_MSVC "${COMPILER_OPTS_MSVC} /DARLANG_BIG_ENDIAN")
else()
    set(COMPILER_OPTS_GCC "${COMPILER_OPTS_GCC} -DARLANG_LITTLE_ENDIAN")
    set(COMPILER_OPTS_MSVC "${COMPILER_OPTS_MSVC} /DARLANG_LITTLE_ENDIAN")
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
add_library(arlang 
    "src/arString.c" 
    "src/arMessage.c" 
    "src/arMemory.c" 
    "src/arMemoryPool.c"
    "src/arStringPool.c"
    "src/arUtils.c"
    "src/arByteStream.c" 
    "src/arStack.c" 
    "src/arThread.c" 
    "src/arProcess.c"
    "src/arLocals.c" 
    "src/arBuilder.c"
    "src/arCompiler.c"
    "src/arPrimitiveValue.c"
    "src/arPrimitiveValue_add.c"
    "src/arPrimitiveValue_sub.c"
    "src/arPrimitiveValue_mult.c"
    "src/arPrimitiveValue_div.c"

    "src/interpreter/lists.c"  
    "src/builder/definition.c" 
    "src/builder/types.c"
    "src/builder/messages.c"

    "src/compiler/syntax_tree.c" 
    "src/compiler/lexer.c" 
    
    "src/compiler/signature.c"
    "src/compiler/messages.c"

    "src/compiler/source_code.c" 
    "src/compiler/syntax_tree_optimize.c"    
    "src/compiler/syntax_tree_compiler.c"
    "src/compiler/syntax_tree_resolve.c"
    "src/compiler/syntax_tree_search.c"
 )
  
set(CMAKE_CXX_STANDARD 20)
add_library(ArlangBuilder
    "source/Builder/Symbols.cpp" 
    "source/Builder/Linker.cpp"
    "source/Builder/LinkError.cpp"
    "source/Builder/MemoryStream.cpp" 
    "source/Builder/Instructions.cpp"
)
add_library(ArlangCompilation
    "source/Compilation/SyntaxTreeNodePackage.cpp" 
    "source/Compilation/SyntaxTreeNodeRef.cpp" 
    "source/Compilation/SyntaxTreeNodeOpBinop.cpp"
    "source/Compilation/SyntaxTreeNodeOpUnaryop.cpp" 
    "source/Compilation/SyntaxTreeNodeFuncDef.cpp" 
    "source/Compilation/Lexer.cpp" 
    "source/Compilation/Compiler.cpp"
    "source/Compilation/SyntaxTreeNodePrimitive.cpp" 
    "source/Compilation/ParseError.cpp" 
    "source/Compilation/SourceCodeParser.cpp" 
    "source/Compilation/SyntaxTreeNodeImport.cpp"
    "source/Compilation/SyntaxTreeNodeFuncBody.cpp" 
    "source/Compilation/SyntaxTreeNodeFuncArg.cpp"
    "source/Compilation/SyntaxTreeNodeFuncRet.cpp"
    "source/Compilation/SyntaxTreeNodeTypeRef.cpp"
    "source/Compilation/SyntaxTreeNodeOpReturn.cpp"
)
add_library(ArlangInterpreter
    "source/Interpreter/Thread.cpp" 
    "source/Interpreter/ThreadStack.cpp"
    "source/Interpreter/Process.cpp" 
    "source/Interpreter/ProcessError.cpp" 
    "source/Interpreter/ThreadError.cpp" 
    "source/Interpreter/Error.cpp"
)
# Tests
add_executable(ArlangTest 
    "source/Tests/SuiteBuilder.cpp"
    "source/Tests/main.cpp"
 "source/Tests/SuiteCompilation.cpp")
target_link_libraries(ArlangTest ArlangInterpreter ArlangCompilation ArlangBuilder)

# Tests
include_directories("${VM_INCLUDE_DIRS}")
add_executable(test "test/main.cpp" "test/test.hpp" "test/utils.hpp" "test/suite_builder.cpp" "test/suite_compiler.cpp")
target_link_libraries(test arlang)
