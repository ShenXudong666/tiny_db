project(tiny_db)


# 工程的源码，不需要包含头文件，当然你也可以包含
set(TINY_DB_SOURCE 
BPTree.cpp
rwdata.cpp
test_myTree.cpp
DataBase.cpp
)

add_executable(${PROJECT_NAME} ${TINY_DB_SOURCE})


# 添加头文件目录，这里说明头文件会自动在这个cmake文件的同级目录下面
target_include_directories(${PROJECT_NAME} PUBLIC .)

# 如果你需要添加额外的二进制文件，在这里写入，会自动复制到可执行文件目录下面
set(COPY_BINARY_FILE 
 ${CMAKE_CURRENT_SOURCE_DIR}/example.bin

)

# 复制的具体命令
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${COPY_BINARY_FILE}
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)