cmake_minimum_required(VERSION 3.28)
project(PoissonReconCGAL)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)

# 添加编译器标志
if(MSVC)
    add_compile_options(/bigobj)
else()
    add_compile_options(-Wa,-mbig-obj)
endif()

find_package(CGAL REQUIRED OPTIONAL_COMPONENTS Qt6)
find_package(CGAL CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(OpenMP REQUIRED)




file(GLOB EXAMPLES "examples/*.h")
file(GLOB GEOMETRY "geometry/*.h")
file(GLOB UTILS "utils/*.h" "utils/*.cpp")
file(GLOB_RECURSE SERVICE "service/*.h" "service/*.cpp")

add_executable(PoissonReconCGAL
        main.cpp
        ${EXAMPLES}
        ${GEOMETRY}
        ${UTILS}
        ${SERVICE}
)

add_subdirectory(third_party)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/Tetgen
)

if(CGAL_Qt5_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC CGAL::CGAL_Basic_viewer)
else()
    message(STATUS "NOTICE: Several examples require Qt5 and will not be compiled.")
endif()

if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
endif()


target_link_libraries(${PROJECT_NAME} PRIVATE CGAL::CGAL)
target_link_libraries(${PROJECT_NAME} PRIVATE TetGen)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)