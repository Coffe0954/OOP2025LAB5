cmake_minimum_required(VERSION 3.10)
project(Lab5_Queue)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab5
    main.cpp
    fixed_block_memory_resource.h
    queue.h
)

target_compile_features(lab5 PRIVATE cxx_std_17)