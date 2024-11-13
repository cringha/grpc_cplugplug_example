 
# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.5)

cmake_policy(SET CMP0048 NEW) # CMake 3.6 required

if(WIN32)
  cmake_policy(SET CMP0074 NEW) # CMake 3.12 required

else(WIN32)

	set (CMAKE_C_COMPILER "/usr/bin/gcc")
	set (CMAKE_CXX_COMPILER "/usr/bin/g++")

endif(WIN32)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

if(WIN32)

else(WIN32)

	if (CMAKE_BUILD_TYPE MATCHES "Debug")
		set(CMAKE_PREFIX_PATH "$ENV{GRPC_HOME_DEBUG}")
	else ()
		set(CMAKE_PREFIX_PATH "$ENV{GRPC_HOME_RELEASE}")
	endif ()

endif(WIN32)

# 项目信息
project(camera-service VERSION 1.1)

    message(STATUS "Echo Using protobuf ${CMAKE_PREFIX_PATH} ")
 
include(./grpc/common.cmake)




# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
# aux_source_directory(. DIR_SRCS)

# 添加 math 子目录
add_subdirectory(src)
