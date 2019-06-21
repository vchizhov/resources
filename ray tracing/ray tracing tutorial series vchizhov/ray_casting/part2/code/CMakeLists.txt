cmake_minimum_required(VERSION 3.6)

project(HydrogenCG_Part0_5)


file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(FILTER sources EXCLUDE REGEX "build/")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources})
add_executable (HydrogenCG_Part0_5 ${sources})
target_compile_features(HydrogenCG_Part0_5 PUBLIC cxx_std_17)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HydrogenCG_Part0_5)