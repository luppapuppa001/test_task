cmake_minimum_required(VERSION 3.15)
project(Test_Task_Alor_Brocker)

set(CMAKE_CXX_STANDARD 23)

find_package(mongocxx REQUIRED)

add_executable(mongo_main
        src/main.cpp
        tests/Test_functions.h
)

target_include_directories(mongo_main PRIVATE
        include
)

target_link_libraries(mongo_main PRIVATE
        mongo::mongocxx_shared
)

add_executable(mongo_tests
        src/Test_main.cpp
        tests/Test_functions.cpp
)

target_include_directories(mongo_tests PRIVATE
        include
        tests
)

target_link_libraries(mongo_tests PRIVATE
        mongo::mongocxx_shared
)