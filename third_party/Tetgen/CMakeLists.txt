cmake_minimum_required(VERSION 3.28)

project(TetGen)

# 定义源文件
set(SOURCES tetgen.cxx predicates.cxx)
# 定义头文件
set(HEADERS tetgen.h)

# 创建一个静态库
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

# 设置包含目录，用于查找头文件
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES "COMPILE_DEFINITIONS" TETLIBRARY)