cmake_minimum_required(VERSION 3.28)
project(PoissonReconCGAL)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)
SET(CGAL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/CGAL-6.0.1)
SET(CGAL_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/CGAL-6.0.1)

# 手动设置 GMP 的路径
if(APPLE)
    set(GMP_INCLUDE_DIR "/opt/homebrew/include")  # GMP 的头文件路径
    set(GMP_LIBRARY "/opt/homebrew/lib/libgmp.dylib")  # GMP 的库文件路径
    SET(MPFR_LIBRARY "/opt/homebrew/lib/libmpfr.dylib")  # MPFR 的库文件路径
elseif (MSVC)
    set(GMP_INCLUDE_DIR "D:/Programs/vcpkg/installed/x64-windows/include")
    set(GMP_LIBRARY "D:/Programs/vcpkg/installed/x64-windows/lib/gmp.lib")
    set(MPFR_LIBRARY "D:/Programs/vcpkg/installed/x64-windows/lib/mpfr.lib")
endif()

# 添加编译器标志
if(MSVC)
    add_compile_options(/bigobj)
elseif (APPLE)
    add_compile_options(-Wl)
else()
    add_compile_options(-Wa,-mbig-obj)
endif()

#find_package(CGAL REQUIRED OPTIONAL_COMPONENTS Qt6)
#find_package(CGAL CONFIG REQUIRED)
if(APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(MPFR REQUIRED mpfr)
endif()
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(assimp CONFIG REQUIRED)

if(MSVC)
    find_package(OpenMP REQUIRED)
endif ()
find_package(GTest REQUIRED)

# 包含 GMP 的头文件路径
include_directories(${GMP_INCLUDE_DIR} ${MPFR_INCLUDE_DIRS})

file(GLOB EXAMPLES "examples/*.h")
file(GLOB GEOMETRY "geometry/*.h")
file(GLOB MODEL "model/*.h")
file(GLOB UTILS "utils/*.h" "utils/*.cpp")
file(GLOB_RECURSE SERVICE "*.h" "*.cpp")
file(GLOB TESTS "tests/*.cpp")

add_executable(PoissonReconCGAL
        main.cpp
        ${EXAMPLES}
        ${GEOMETRY}
        ${MODEL}
        ${UTILS}
        ${SERVICE}
)

add_subdirectory(third_party)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/Tetgen
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/CGAL-6.0.1/include
)

if(CGAL_Qt5_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC CGAL::CGAL_Basic_viewer)
else()
    message(STATUS "NOTICE: Several examples require Qt5 and will not be compiled.")
endif()

if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
endif()


# target_link_libraries(${PROJECT_NAME} PRIVATE CGAL::CGAL)
target_link_libraries(${PROJECT_NAME} PRIVATE TetGen)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${GMP_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${MPFR_LIBRARY})

# 创建测试可执行文件
add_executable(Tests ${TESTS})
target_include_directories(Tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/Tetgen
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/CGAL-6.0.1/include
)
if(CGAL_Qt5_FOUND)
    target_link_libraries(Tests PUBLIC CGAL::CGAL_Basic_viewer)
else()
    message(STATUS "NOTICE: Several examples require Qt5 and will not be compiled.")
endif()

target_link_libraries(Tests PRIVATE
        # CGAL::CGAL
        TetGen
        assimp::assimp
        ${GTEST_LIBRARIES}
        ${GMP_LIBRARY}
        ${MPFR_LIBRARY}
        # pthread
)
if(APPLE)
    target_link_libraries(Tests PRIVATE pthread)
endif ()

# 运行测试
enable_testing()
add_test(NAME MyTests COMMAND Tests)