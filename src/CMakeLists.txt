

# cmake_minimum_required (VERSION 3.5)
# project(sub)
 


# # 查找当前目录下的所有源文件
# # 并将名称保存到 DIR_SRCS 变量
# aux_source_directory(. DIR_WIN_SRCS)




# 指定生成目标 


#这是子目录的CMakefile, 编译过程中的提示信息
message(STATUS "Enter camera-service  sub dir...")
message(STATUS "555555555 PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}  ")
message(STATUS "555555555 CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}  ")
 
#要点1:这个MODULE_MQTT需要在源代码根目录被引用
# set(MODULE_MQTT gpMqtt)
 
#要点2:通过这个set语句。${MODULE_MQTT}这个模块才会被上层应用访问到
# set(MODULE_MQTT ${MODULE_MQTT} PARENT_SCOPE)
 
#这个是普通做法，指示当前目录下的所有源代码参与编译add_library.无需逐个文件手工指定
aux_source_directory (. SOURCES_LIDAR_COL)
 
#要点3：子目录在要点1被定义为一个子模块，为了参与主目录的工程的编译，它需要被编译为一个动态库或者静态库：动态库:SHARED|静态库：STATIC
# add_library(${MODULE_MQTT} STATIC ${SOURCES_MQTT})
 

include_directories(${PROJECT_SOURCE_DIR}/src)
#include_directories(${PROJECT_SOURCE_DIR}/3rd/jsoncpp/include)
#include_directories(${PROJECT_SOURCE_DIR}/3rd/libzmq/include)
#include_directories(${PROJECT_SOURCE_DIR}/3rd/cmdline)
include_directories(${PROJECT_SOURCE_DIR}/3rd/spdlog)
#include_directories(${PROJECT_SOURCE_DIR}/3rd/mqtt/inc)
#include_directories(${PROJECT_SOURCE_DIR}/3rd/websocket)
#include_directories(${PROJECT_SOURCE_DIR}/3rd/asio.1.30)



 
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	
	message(STATUS "99999 include  /opt/MVS/include ")
	include_directories(/opt/MVS/include)
message(STATUS "99999 include /opt/MVS/lib/64 ")
	link_directories(/opt/MVS/lib/64) 
	
	
endif( )


if (CMAKE_BUILD_TYPE MATCHES "Debug")
	
	# link_directories( ${PROJECT_SOURCE_DIR}/3rd/libzmq/lib)
	# link_directories( ${PROJECT_SOURCE_DIR}/3rd/jsoncpp/lib/Debug)
	# link_directories( ${PROJECT_SOURCE_DIR}/3rd/mqtt/debug)
	 
else ()
    # link_directories( ${PROJECT_SOURCE_DIR}/3rd/libzmq/lib)
	# link_directories( ${PROJECT_SOURCE_DIR}/3rd/jsoncpp/lib/Release)
	# link_directories( ${PROJECT_SOURCE_DIR}/3rd/mqtt/release)
	
endif ()

 
 
# add_definitions("-DDISABLE_PCAP_PARSE")
# add_definitions("-D_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING")
# add_definitions("-DZMQ_STATIC")
# add_definitions("-D_CRT_SECURE_NO_WARNINGS")

# add_definitions("-DASIO_STANDALONE")
# add_definitions("-D_WEBSOCKETPP_CPP11_TYPE_TRAITS_")

# add_definitions("-D_HAS_ITERATOR_DEBUGGING=1")
# add_definitions("-D_ITERATOR_DEBUG_LEVEL=2")


add_executable(camera-service  camera_grpc_server.cpp  ${SOURCES_LIDAR_COL})

target_link_libraries(camera-service   -static-libgcc;-static-libstdc++ )

# target_compile_definitions(camera-service   DEV_CODE=1)

# ../../3rd/libzmq/lib;../../3rd/jsoncpp/lib/Release
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
# target_link_libraries(camera-service libpaho-mqtt3c.so)
# 添加链接库 Ws2_32.lib;libzmq-v143-mt-sgd-4_3_6.lib;jsoncpp_static.lib;iphlpapi.lib
# target_link_libraries(camera-service libzmq.so)
# target_link_libraries(camera-service libzmq-v143-mt-s-4_3_6)
# target_link_libraries(camera-service libjsoncpp.so)
# target_link_libraries(camera-service win/lidar_collector)

	target_link_libraries(camera-service libMvCameraControl.so )
	# target_link_libraries(camera-service /opt/MVS/lib/libMvCameraControl.so )
	message(STATUS "99999 target_link_libraries libMvCameraControl.so ")
else()


	if (CMAKE_BUILD_TYPE MATCHES "Debug")
	 
	    # add_library		
	 
#		target_link_libraries(camera-service   libzmq-v143-mt-sgd-4_3_6.lib )

#		target_link_libraries(camera-service   jsoncpp_static.lib )
			
#		target_link_libraries(camera-service    paho-mqtt3c-static.lib)
		 
	else ()
#		target_link_libraries(camera-service  libzmq-v143-mt-s-4_3_6.lib)

#		target_link_libraries(camera-service   jsoncpp_static.lib )
			
#		target_link_libraries(camera-service   paho-mqtt3c-static.lib)

	endif ()



endif()



# System level library 
#target_link_libraries(camera-service ws2_32)
#target_link_libraries(camera-service iphlpapi)
target_link_libraries(camera-service
    absl::check
    absl::flags
    absl::flags_parse
    absl::log
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
	


# target_link_libraries(camera-service ${MODULE_MQTT})
