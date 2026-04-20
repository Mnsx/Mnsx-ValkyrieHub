cmake_minimum_required(VERSION 3.10)
project(Mnsx-HermesRPC LANGUAGES CXX)

find_package(Threads REQUIRED)

file(GLOB_RECURSE HERMES_SRC "src/*.cpp")
add_library(HermesRPC STATIC ${HERMES_SRC})

set_target_properties(HermesRPC PROPERTIES AUTOMOC OFF AUTOUIC OFF AUTORCC OFF)

target_include_directories(HermesRPC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(HermesRPC PUBLIC AchillesLink KrakenPool ArgusLog Threads::Threads)

if(MSVC)
    add_compile_options(/utf-8)
endif()