cmake_minimum_required(VERSION 2.8)
project(gpgpu_hf)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb -Wall -Wextra -pedantic")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/attila/projects/gpgpu_hf/")

set(SOURCE_FILES
    clwrapper.cpp
    clwrapper.hpp
    main.cpp
    ObjLoader.hpp
    ObjLoader.cpp)

add_executable(gpgpu_hf ${SOURCE_FILES} CLBuffer.hpp CLKernel.hpp SpringyObject.cpp SpringyObject.hpp Camera.cpp Camera.hpp AbstractObject.hpp VolumeMesh.cpp VolumeMesh.hpp Sphere.cpp Sphere.hpp)

target_link_libraries (gpgpu_hf OpenCL SDL2 GL GLU)
