cmake_minimum_required(VERSION 3.10)
project(tiny_db)


include_directories(./tiny_db)
add_executable(tiny_db 
./tiny_db/BPlusTree.h ./tiny_db/BPulsTree.cpp ./tiny_db/Demo.cpp
./tiny_db/rwdata.cpp ./tiny_db/rwdata.h ./tiny_db/test_struct.cpp ./tiny_db/test.cpp
)