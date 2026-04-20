cmake_minimum_required(VERSION 3.10)
project(AchillesLink LANGUAGES CXX)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
add_library(AchillesLink STATIC ${SRC_FILES})

set_target_properties(AchillesLink PROPERTIES AUTOMOC OFF AUTOUIC OFF AUTORCC OFF)

target_include_directories(AchillesLink PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(AchillesLink PUBLIC ArgusLog)
target_compile_features(AchillesLink PUBLIC cxx_std_11)