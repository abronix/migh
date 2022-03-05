cmake_minimum_required(VERSION 3.11)

project(snowfall)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/scripts")
set(3RD "${CMAKE_SOURCE_DIR}/third_party/installed")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(ensure_output_path)

if(WIN32)
  set(FLAG_SUFF ${CMAKE_BUILD_TYPE})
  string(TOUPPER ${FLAG_SUFF} FLAG_SUFF)
  set(CMAKE_C_FLAGS_${FLAG_SUFF} "${CMAKE_C_FLAGS_${FLAG_SUFF}} /MD /MP")
  set(CMAKE_CXX_FLAGS_${FLAG_SUFF} "${CMAKE_CXX_FLAGS_${FLAG_SUFF}} /MD /MP")
  set(CMAKE_CXX_STANDARD 17)
  add_definitions(-DWIN32 -D_WIN32 -DWINDOWS -D_WINDOWS -D_WIN32_WINNT=0x0601 -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++17 -lpthread -lrt -ldl -Wno-deprecated-declarations")
endif()

## this will affect find_package (in CMake 3.12 and above)
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

## some functions will convert relative paths to absolute (in CMake 3.13 and above)
if(POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()

set(BOOST_ROOT "${3RD}")
set(BOOST_LIBRARYDIR "${3RD}")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_DEBUG OFF)
set(Boost_COMPILER vc169)
find_package(Boost 1.77 REQUIRED COMPONENTS atomic chrono date_time filesystem program_options regex serialization system thread timer iostreams)
add_definitions(-D__STDC_FORMAT_MACROS -DBOOST_ALL_NO_LIB -DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS -DBOOST_MPL_LIMIT_LIST_SIZE=50)

add_subdirectory(research/mi_container)
add_subdirectory(research/concurrent_hashmap)
