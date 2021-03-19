cmake_minimum_required(VERSION 3.0)

project(interClient)

# set( CMAKE_SYSTEM_NAME Linux )
# set( CMAKE_SYSTEM_PROCESSOR aarch64 )

# specify the cross compiler
# set( CMAKE_C_COMPILER aarch64-linux-gnu-gcc )
# set( CMAKE_CXX_COMPILER aarch64-linux-gnu-g++ )
set(CMAKE_CXX_STANDARD 11)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_library(
  LIBV4L2
  NAMES libv4l2.so
  HINTS /usr/lib/x86_64-linux-gnu
  REQUIRED)

find_library(
  LIBUDEV
  NAMES libudev.so
  HINTS /usr/lib/x86_64-linux-gnu
  REQUIRED)

add_subdirectory(v4l2)
ADD_LIBRARY(interCommu STATIC ${CMAKE_SOURCE_DIR}/../interCommu/interCommunication.cpp)
# ADD_LIBRARY(USBVIDEO STATIC )

# set( OpenCV_DIR[OpenCV.cmake PATH] )
# find_package( OpenCV REQUIRED )
# include_directories( ${OpenCV_INCLUDE_DIR} )

add_executable( interClient
    main.cpp
)

#set(OpenCV_DIR [OpenCV.cmake PATH])

target_link_libraries(interClient USBVIDEO
                                ${LIBV4L2}
                                ${LIBUDEV}
                                interCommu
                                # ${OpenCV_LIBRARIES}
                                # Threads::Threads
                                )
