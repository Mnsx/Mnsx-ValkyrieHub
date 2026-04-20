cmake_minimum_required(VERSION 3.10)
project(ArgusLog LANGUAGES CXX)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
add_library(ArgusLog STATIC ${SRC_FILES})

target_include_directories(ArgusLog PUBLIC include)
target_include_directories(ArgusLog PRIVATE .)

target_compile_features(ArgusLog PUBLIC cxx_std_11)