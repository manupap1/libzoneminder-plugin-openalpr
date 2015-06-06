# Main CMake file for the libzoneminder-plugin-openalpr project.
# Created by Emmanuel Papin (manupap01@gmail.com)

cmake_minimum_required(VERSION 2.6)
project(libzm_plugin_openalpr)
set(libzm_plugin_openalpr_VERSION "1.0.0")

# CMake does not allow out-of-source build if CMakeCache.exists in the source
# folder, abort and notify the user
if(
    (NOT (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR))
    AND (EXISTS "${CMAKE_SOURCE_DIR}/CMakeCache.txt"))
        message(FATAL_ERROR " You are attempting to do an out-of-source build,-
            but a cmake cache file for an in-source build exists. Please delete-
            the file CMakeCache.txt from the source folder to proceed.")
endif(
    (NOT (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR))
    AND (EXISTS "${CMAKE_SOURCE_DIR}/CMakeCache.txt"))

# Default build type. To change the build type, use the CMAKE_BUILD_TYPE
# configuration option.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
        Release CACHE STRING "Build type: Release or Debug" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Can assist in troubleshooting
#set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_INSTALL_ALWAYS ON)

# Default CLFAGS and CXXFLAGS:
set(CMAKE_C_FLAGS_RELEASE "-Wall -D__STDC_CONSTANT_MACROS -O2")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -D__STDC_CONSTANT_MACROS -O2")
set(CMAKE_C_FLAGS_DEBUG "-Wall -D__STDC_CONSTANT_MACROS -g")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -D__STDC_CONSTANT_MACROS -g")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Modules that we need:
include (GNUInstallDirs)
include (CheckIncludeFile)
include (CheckIncludeFileCXX)
include (CheckIncludeFiles)
include (CheckFunctionExists)
include (CheckTypeSize)
include (CheckStructHasMember)
include (FindPkgConfig)

if(PKG_CONFIG_FOUND)
    pkg_search_module(libzoneminder libzoneminder>=1.28.1)
    if(libzoneminder_FOUND)
        execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} libzoneminder
            --variable=pkglibdir OUTPUT_VARIABLE zm_pluginslibdir
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(NOT zm_pluginslibdir)
            message(FATAL_ERROR "Unable to find plugin library directory")
        endif(NOT zm_pluginslibdir)
        execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} libzoneminder
            --variable=sysconfdir OUTPUT_VARIABLE zm_pluginsconfdir
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(NOT zm_pluginsconfdir)
            message(FATAL_ERROR "Unable to find plugin configuration directory")
        endif(NOT zm_pluginsconfdir)
        execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} libzoneminder
            --variable=webdir OUTPUT_VARIABLE zm_pluginswebdir
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(NOT zm_pluginswebdir)
            message(FATAL_ERROR "Unable to find plugin web directory")
        endif(NOT zm_pluginswebdir)
    else(libzoneminder_FOUND)
        message(FATAL_ERROR "libzoneminder.pc not found")
    endif(libzoneminder_FOUND)
    pkg_search_module(opencv opencv>=2.3.1)
    if(NOT opencv_FOUND)
        message(FATAL_ERROR "opencv.pc not found")
    endif(NOT opencv_FOUND)
else(PKG_CONFIG_FOUND)
    message(FATAL_ERROR "pkg-config not found")
endif(PKG_CONFIG_FOUND)

find_package(OpenCV COMPONENTS core imgproc REQUIRED)
if(OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIR})
else(OpenCV_FOUND)
    message(FATAL_ERROR "Cannot build application without OpenCV.
        Please set OpenCV_INCLUDE_DIR.")
endif(OpenCV_FOUND)

find_package(Boost COMPONENTS program_options REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
else(Boost_FOUND)
    message(FATAL_ERROR "Cannot build application without Boost.
        Please set Boost_INCLUDE_DIR.")
endif(Boost_FOUND)

check_include_file_cxx("alpr.h" HAVE_ALPR_H)
if(NOT HAVE_ALPR_H)
    message(FATAL_ERROR "Missing include file alpr.h")
endif(NOT HAVE_ALPR_H)
check_include_file_cxx("opencv2/core/core.hpp"
    HAVE_OPENCV2_CORE_CORE_HPP)
if(NOT HAVE_OPENCV2_CORE_CORE_HPP)
    message(FATAL_ERROR "Missing include file opencv2/core/core.hpp")
endif(NOT HAVE_OPENCV2_CORE_CORE_HPP)
check_include_file_cxx("opencv2/imgproc/imgproc.hpp"
    HAVE_OPENCV2_IMGPROC_IMGPROC_HPP)
if(NOT HAVE_OPENCV2_IMGPROC_IMGPROC_HPP)
    message(FATAL_ERROR "Missing include file opencv2/imgproc/imgproc.hpp")
endif(NOT HAVE_OPENCV2_IMGPROC_IMGPROC_HPP)
check_include_file_cxx("boost/algorithm/string.hpp"
    HAVE_BOOST_ALGORITHM_STRING_HPP)
if(NOT HAVE_BOOST_ALGORITHM_STRING_HPP)
    message(FATAL_ERROR "Missing include file boost/algorithm/string.hpp")
endif(NOT HAVE_BOOST_ALGORITHM_STRING_HPP)
check_include_file_cxx("boost/program_options.hpp"
    HAVE_BOOST_PROGRAM_OPTIONS_HPP)
if(NOT HAVE_BOOST_PROGRAM_OPTIONS_HPP)
    message(FATAL_ERROR "Missing include file boost/program_options.hpp")
endif(NOT HAVE_BOOST_PROGRAM_OPTIONS_HPP)

# Process subdirectories
add_subdirectory(data)
add_subdirectory(src)
