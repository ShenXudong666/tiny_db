cmake_minimum_required(VERSION 3.20)

project(tiny_db_project)

if(MSVC)
    #utf-8编码  
    add_compile_options(/utf-8)
endif()

add_subdirectory(tiny_db)