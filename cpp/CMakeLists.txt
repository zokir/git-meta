cmake_minimum_required(VERSION 3.1)

#TODO: How should we go about libgit2 library? Add as submodule and built here?
# or make user set include and lib varibles for libgit2

project(qmeta)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQURED ON) # to prevent fallback to older standards
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost 1.56 REQUIRED COMPONENTS filesystem system program_options)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(lib/core)
add_subdirectory(lib/util)
add_subdirectory(lib/cmd)

set(target qmeta) # create variable target
add_executable(${target} qmeta.cpp)

target_link_libraries(${target} ${Boost_LIBRARIES} cmd util core)

install(TARGETS ${target} DESTINATION bin)
