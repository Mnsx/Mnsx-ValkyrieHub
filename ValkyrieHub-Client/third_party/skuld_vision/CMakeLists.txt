cmake_minimum_required(VERSION 3.10)
project(Mnsx-SkuldVision LANGUAGES CXX)

find_package(OpenCV REQUIRED)

add_library(SkuldVision STATIC src/SkuldVision.cpp src/Inspector.cpp)

set_target_properties(SkuldVision PROPERTIES AUTOMOC OFF AUTOUIC OFF AUTORCC OFF)

target_include_directories(SkuldVision PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(SkuldVision PRIVATE ${OpenCV_LIBS})