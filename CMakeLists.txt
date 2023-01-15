cmake_minimum_required(VERSION 3.16)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/scc/cmake)
cmake_policy(SET CMP0110 NEW)

project(VPVPer_Test)

option(FULL_TEST_SUITE "enable also long-running tests" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy checks" OFF)

include(ConanInline)
include(GNUInstallDirs)
include(BuildType)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)

if(COMPILER_SUPPORTS_MARCH_NATIVE)
	if("${CMAKE_BUILD_TYPE}" STREQUAL "") 
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
	elseif(NOT(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
	endif()
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Werror")
    set(CMAKE_CXX_FLAG_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_C_FLAG_RELEASE "-O3 -DNDEBUG")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(warnings "/W4 /WX /EHsc")
endif()

if(ENABLE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
    set(COVERAGE_EXCLUDES "osci-lib/scc/*" "/engr/dev/tools/*")
endif()

find_program(CLANG_TIDY_EXE NAMES "clang-tidy-9")
if(ENABLE_CLANG_TIDY)
if(CLANG_TIDY_EXE)
  	message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
  	set(CLANG_TIDY_CHECKS "-*,modernize-*,-modernize-use-trailing-return-type,clang-analyzer-core.*,clang-analyzer-cplusplus.*")
  	set(CMAKE_CXX_CLANG_TIDY 
  		${CLANG_TIDY_EXE};
  		-checks=${CLANG_TIDY_CHECKS};
  		-fix;)
else()
  	message(AUTHOR_WARNING "clang-tidy not found!")
  	set(CMAKE_CXX_CLANG_TIDY "" CACHE STRING "" FORCE) # delete it
endif()
endif()

conan_check()
conan_configure(REQUIRES fmt/8.0.1 spdlog/1.9.2 boost/1.75.0 gsl-lite/0.37.0 systemc/2.3.3 catch2/3.1.0 zlib/1.2.11 lz4/1.9.4
                GENERATORS cmake_find_package
                OPTIONS fmt:header_only=True spdlog:header_only=True 
                )
conan_install()
set(CONAN_CMAKE_SILENT_OUTPUT ON)
find_package(ZLIB)
find_package(lz4)
# This line finds the boost lib and headers. 
set(Boost_NO_BOOST_CMAKE ON) #  Don't do a find_package in config mode before searching for a regular boost install.
find_package(Boost COMPONENTS program_options filesystem system thread REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Catch2)

include(SystemCPackage)
include(CTest)
include(Catch)

enable_testing()

add_subdirectory(scc)
add_subdirectory(vpvper)
add_subdirectory(src)
add_subdirectory(tests)
