cmake_minimum_required(VERSION 3.21)
project(cmake_tools)

include(CMakePrintHelpers)
cmake_print_variables(PROJECT_SOURCE_DIR)

file(GLOB INCLUDE_CMAKE_LIST ${PROJECT_SOURCE_DIR}/*/*.cmake)
foreach(sub_file ${INCLUDE_CMAKE_LIST})
    file(RELATIVE_PATH relative_sub_file ${CMAKE_CURRENT_SOURCE_DIR} ${sub_file}) # 获取相对路径
    cmake_print_variables(relative_sub_file)
    include(${relative_sub_file})
endforeach()