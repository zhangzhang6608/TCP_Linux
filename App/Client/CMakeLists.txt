#获取服务端的源文件
file(GLOB Client_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
message(STATUS "匹配到的源文件：${Client_SRCS}")

#创建服务端可执行文件
add_executable(client ${Client_SRCS})


target_include_directories(client 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # src/include 目录
)

#设置编译选项
target_compile_options(client PRIVATE -std=c++11 -Wall)

# 设置 server 可执行文件输出目录
set_target_properties(client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY 
    ${PROJECT_SOURCE_DIR}/bin
    OUTPUT_NAME "Client_Block")

