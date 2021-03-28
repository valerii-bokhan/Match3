cmake_minimum_required(VERSION 3.10)

project(Match3)

file(GLOB SOURCES "Match3/*.*")
 
add_executable(Match3 ${SOURCES})

target_compile_features(Match3 PUBLIC cxx_std_17)
set_target_properties(Match3 PROPERTIES CXX_EXTENSIONS OFF)