cmake_minimum_required(VERSION 3.10)

project(Match3)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS Match3/*.cpp Match3/*.h)

include_directories(Match3)
 
add_executable(Match3 ${SOURCES})

target_compile_features(Match3 PUBLIC cxx_std_17)
set_target_properties(Match3 PROPERTIES CXX_EXTENSIONS OFF)