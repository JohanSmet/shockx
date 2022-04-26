cmake_minimum_required(VERSION 3.12)

project(shockx)

# export a JSON compilation database for clangd
set (CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# libraries
add_subdirectory(ShockMac/Libraries)
