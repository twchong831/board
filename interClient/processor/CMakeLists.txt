cmake_minimum_required(VERSION 3.0)

add_library(PROCESSOR_CARNAVI_LiDAR STATIC
    carnavicomProcessor.h
    carnavicomProcessor.cpp)

#target_link_libraries( PROCESSOR_CARNAVI_LiDAR LIDARPARSER UDP )
