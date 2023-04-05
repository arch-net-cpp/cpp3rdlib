set(CURRENT_LIB_PATH ${CMAKE_CURRENT_LIST_DIR})
cmake_print_variables(CURRENT_LIB_PATH)
set(ZLIB_INCLUDE_DIR ${CURRENT_LIB_PATH}/include/)
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})

find_library(ZLIB_LIBRARIES NAMES libz.a PATHS ${CURRENT_LIB_PATH}/lib REQUIRED)


add_library(zlib STATIC IMPORTED GLOBAL)
set_target_properties(zlib PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIR}"
        IMPORTED_LOCATION "${ZLIB_LIBRARIES}"
        )