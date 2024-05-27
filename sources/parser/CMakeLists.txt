# minimum required CMAKE version
CMAKE_MINIMUM_REQUIRED(VERSION 3.7 FATAL_ERROR)

set(target ${PROJ_LIBRARY}_parser)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


set(CMAKE_CXX_STANDARD 20)

# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)

# Specify the version of the antlr4 library needed for this project.
# By default the latest version of antlr4 will be used.  You can specify a
# specific, stable version by setting a repository tag value or a link
# to a zip file containing the libary source.
# set(ANTLR4_TAG 4.13.1)
# set(ANTLR4_ZIP_REPOSITORY https://github.com/antlr/antlr4/archive/refs/tags/4.13.1.zip)

# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})

# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/bin/antlr-4.13.1-complete.jar)
# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

# Call macro to add lexer and grammar to your build dependencies.
antlr_target(PascalLexer ${CMAKE_CURRENT_SOURCE_DIR}/grammar/PascalLexer.g4 LEXER
        PACKAGE ANTLRPascalParser)
antlr_target(PascalParser ${CMAKE_CURRENT_SOURCE_DIR}/grammar/PascalParser.g4 PARSER
        VISITOR LISTENER
        PACKAGE ANTLRPascalParser
        DEPENDS_ANTLR PascalLexer
        COMPILE_FLAGS -lib ${ANTLR_PascalLexer_OUTPUT_DIR})

# include generated files in project environment
include_directories(${ANTLR_PascalLexer_OUTPUT_DIR})
include_directories(${ANTLR_PascalParser_OUTPUT_DIR})
message(STATUS "${ANTLR_PascalLexer_OUTPUT_DIR}}")

file(GLOB hdrs "*.h*")
file(GLOB srcs "*.cpp")

# add generated grammar to demo binary target
add_library(${target} STATIC ${srcs} ${hdrs}
        ${ANTLR_PascalLexer_CXX_OUTPUTS}
        ${ANTLR_PascalParser_CXX_OUTPUTS})
target_link_libraries(${target} ${LIBRARY_DEPS} antlr4_static)
