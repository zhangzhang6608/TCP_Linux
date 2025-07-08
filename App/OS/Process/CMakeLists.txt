#获取服务端的源文件
file(GLOB Process_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
message(STATUS "匹配到的源文件：${Process_SRCS}")

#创建服务端可执行文件
add_executable(Process ${Process_SRCS})


target_include_directories(Process 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # src/include 目录
)

#设置编译选项
target_compile_options(Process PRIVATE -std=c++11 -Wall)

# 设置 server 可执行文件输出目录
set_target_properties(Process PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY 
    ${PROJECT_SOURCE_DIR}/bin/OS
    OUTPUT_NAME "OS_Process")
