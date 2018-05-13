cmake_minimum_required(VERSION 3.1)

#TODO: How should we go about libgit2 library? Add as submodule and built here?
# or make user set include and lib varibles for libgit2

project(qmeta)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQURED ON) # to prevent fallback to older standards
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost 1.56 REQUIRED COMPONENTS filesystem system program_options)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(lib/core)
add_subdirectory(lib/util)

set(target qmeta) # create variable target
add_executable(${target} qmeta.cpp)

if(Boost_FOUND)
    target_link_libraries(${target} ${Boost_LIBRARIES})
endif()

target_link_libraries(${target} util core)

install(TARGETS ${target} DESTINATION bin)
