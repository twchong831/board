cmake_minimum_required(VERSION 3.0)

project(interClient)

set( CMAKE_SYSTEM_NAME Linux )
set( CMAKE_SYSTEM_PROCESSOR aarch64 )

# specify the cross compiler
set( CMAKE_C_COMPILER aarch64-linux-gnu-gcc )
set( CMAKE_CXX_COMPILER aarch64-linux-gnu-g++ )
set( CMAKE_CXX_STANDARD 11 )

find_package(Threads)

add_subdirectory( processor )
add_subdirectory( UDP )
add_subdirectory( LiDAR)

ADD_LIBRARY(interCommu STATIC ${CMAKE_SOURCE_DIR}/../interCommu/interCommunication.cpp)

add_executable( interClient
    main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)

target_link_libraries( interClient 
	PROCESSOR_CARNAVI_LiDAR 
	LIDARPARSER 
	UDP
	interCommu
	Threads::Threads )
