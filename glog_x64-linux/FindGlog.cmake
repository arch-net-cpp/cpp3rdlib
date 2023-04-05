### glog
set(CURRENT_LIB_PATH ${CMAKE_CURRENT_LIST_DIR})
cmake_print_variables(CURRENT_LIB_PATH)
set(GLOG_INCLUDE_DIR ${CURRENT_LIB_PATH}/include/)

INCLUDE_DIRECTORIES(${GLOG_INCLUDE_DIR})

find_library(GLOG_LIBRARY NAMES libglog.a PATHS ${CURRENT_LIB_PATH}/lib REQUIRED)

add_library(glog STATIC IMPORTED GLOBAL)
set_target_properties(glog PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${GLOG_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "gflags"
        IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
        IMPORTED_LOCATION "${GLOG_LIBRARY}"
        )
add_dependencies(glog gflags)

cmake_print_variables(GLOG_LIBRARY)
cmake_print_variables(GLOG_INCLUDE_DIR)

