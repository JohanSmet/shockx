cmake_minimum_required(VERSION 3.12)

project(shockx)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_EXTENSIONS OFF)

# options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_UTILS "Build utility programs" OFF)

# export a JSON compilation database for clangd
set (CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# check system libraries
include(CheckSymbolExists)

# >> some platform require an extra link-libraries for math functions and others do not
check_symbol_exists("floor" "math.h" HAVE_FLOOR)
if (NOT HAVE_FLOOR)
	unset(HAVE_FLOOR CACHE)
	list(APPEND CMAKE_REQUIRED_LIBRARIES m)
	check_symbol_exists("floor" "math.h" HAVE_FLOOR)
	if (NOT HAVE_FLOOR)
		message(FATAL_ERROR "Unable to find math functions")
	else()
		list(APPEND LIBS_MATH m)
		list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES m)
		unset(HAVE_FLOOR CACHE)
	endif()
endif()

# libraries
include(ShockMac/Libraries/CMakeLists.txt)

# unit tests
if (BUILD_TESTS)
	include(ShockMac/test/CMakeLists.txt)
endif()
