cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

project(prog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(prog main.cpp)

target_include_directories(prog PUBLIC  "./include")

target_compile_definitions(prog PUBLIC  "$<$<CONFIG:DEBUG>:DEBUG>"
                                        "$<$<CONFIG:RELEASE>:RELEASE>")
