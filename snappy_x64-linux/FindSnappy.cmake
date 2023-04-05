set(CURRENT_LIB_PATH ${CMAKE_CURRENT_LIST_DIR})
cmake_print_variables(CURRENT_LIB_PATH)
set(snappy_INCLUDE_DIR ${CURRENT_LIB_PATH}/include/)
INCLUDE_DIRECTORIES(${snappy_INCLUDE_DIR})

find_library(snappy_LIBRARIES NAMES libsnappy.a PATHS ${CURRENT_LIB_PATH}/lib REQUIRED)

add_library(snappy STATIC IMPORTED GLOBAL)
set_target_properties(snappy PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${snappy_INCLUDE_DIR}"
        IMPORTED_LOCATION "${snappy_LIBRARIES}"
        )
