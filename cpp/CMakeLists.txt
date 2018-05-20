cmake_minimum_required(VERSION 3.1)

#TODO: How should we go about libgit2 library? Add as submodule and built here?
# or make user set include and lib varibles for libgit2

project(qmeta)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQURED ON) # to prevent fallback to older standards
set(CMAKE_CXX_EXTENSIONS OFF)

# if kitware decides one day to incldue libgit2, remove line below/and custom file
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Boost 1.56 REQUIRED COMPONENTS filesystem system program_options)
find_package(Libgit2 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${Boost_INCLUDE_DIRS}
                    ${LIBGIT2_INCLUDE_DIR}
                    )

add_subdirectory(lib/core)
add_subdirectory(lib/util)
add_subdirectory(lib/cmd)

set(target qmeta) # create variable target
add_executable(${target} qmeta.cpp)

target_link_libraries(${target} cmd util core ${Boost_LIBRARIES} ${LIBGIT2_LIBRARIES})

install(TARGETS ${target} DESTINATION bin)
