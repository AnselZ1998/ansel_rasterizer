cmake_minimum_required(VERSION 3.10)

project(Rasterizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 依赖
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

# 源码
add_executable(Rasterizer
    main.cpp
    rasterizer.cpp
    Texture.cpp
    Triangle.cpp

    rasterizer.hpp
    Texture.hpp
    Triangle.hpp
    Shader.hpp
    global.hpp
    OBJ_Loader.h
)

# 头文件路径
target_include_directories(Rasterizer
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}  # 工程根目录
        ${CMAKE_SOURCE_DIR}/nlohmann  # 让 #include "nlohmann/json.hpp" 能找到
)

# 链接库
target_link_libraries(Rasterizer
    PRIVATE
        ${OpenCV_LIBS}
        Eigen3::Eigen
)

# 可选警告
# target_compile_options(Rasterizer PRIVATE -Wall -Wextra -Wpedantic)
