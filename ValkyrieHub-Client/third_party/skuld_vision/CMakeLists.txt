cmake_minimum_required(VERSION 3.10)
project(Mnsx-SkuldVision LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-avx -mno-avx2")

find_package(OpenCV REQUIRED)

add_library(SkuldVision STATIC src/SkuldVision.cpp src/Inspector.cpp)

set_target_properties(SkuldVision PROPERTIES AUTOMOC OFF AUTOUIC OFF AUTORCC OFF)

target_include_directories(SkuldVision PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(SkuldVision PRIVATE ${OpenCV_LIBS})

set(ORT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/onnx")

# 1. 配置 Include 目录 (让代码能找到 onnxruntime_cxx_api.h)
target_include_directories(SkuldVision PRIVATE
        ${ORT_DIR}/include
)

# 2. 配置 Library 目录 (告诉 CMake 去哪里找底层库文件)
target_link_directories(SkuldVision PRIVATE
        ${ORT_DIR}/lib
)

# 3. 配置具体的库名称 (这就是链接 onnxruntime.lib 或 -lonnxruntime)
target_link_libraries(SkuldVision PRIVATE
        onnxruntime
)