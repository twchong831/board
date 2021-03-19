cmake_minimum_required(VERSION 3.0)

project(dcuServer)

#set( CMAKE_SYSTEM_NAME Linux )
#set( CMAKE_SYSTEM_PROCESSOR aarch64 )

# specify the cross compiler
#set( CMAKE_C_COMPILER aarch64-linux-gnu-gcc )
#set( CMAKE_CXX_COMPILER aarch64-linux-gnu-g++ )
set(CMAKE_CXX_STANDARD 11)

ADD_LIBRARY(interCommu STATIC ${CMAKE_SOURCE_DIR}/../interCommu/interCommunication.cpp)
ADD_LIBRARY(UDP STATIC ${CMAKE_SOURCE_DIR}/../interClient/UDP/carnavicom_udp.cpp)

add_executable( dcuServer
    main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)

target_link_libraries( dcuServer
			interCommu 
			UDP)
