CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

set(TOPDIR ${CMAKE_CURRENT_SOURCE_DIR})

set(OUTDIR ${TOPDIR}/build/out/)
set(BUILDDIR ${TOPDIR}/build/)


#SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

add_compile_options("--std=c++11")
#add_compile_options("--std=c++14")
#add_definitions(-DBUILD_SSL)
#add_compile_options(-DBUILD_SSL)

add_subdirectory(${TOPDIR}/src ${BUILDDIR}/src)
add_subdirectory(${TOPDIR}/test ${BUILDDIR}/test)
