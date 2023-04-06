set(CURRENT_LIB_PATH ${CMAKE_CURRENT_LIST_DIR})
cmake_print_variables(CURRENT_LIB_PATH)

set(acl_path ${CURRENT_LIB_PATH}/include/acl/lib_acl)
set(acl_include ${acl_path}/include)
set(master_include ${acl_path}/src/master)

set(acl_cpp_path ${CURRENT_LIB_PATH}/include/acl/lib_acl_cpp)
set(acl_cpp_include ${acl_cpp_path}/include)

set(fiber_path ${CURRENT_LIB_PATH}/include/acl/lib_fiber/c)
set(fiber_include ${fiber_path}/include)

set(fiber_cpp_path ${CURRENT_LIB_PATH}/include/acl/lib_fiber/cpp)


set(LIBFIBER_INCLUDE_DIR 
        ${fiber_include}
        ${fiber_cpp_path}/src
        ${fiber_cpp_path}/include
        ${acl_include}
        ${master_include}
        ${acl_cpp_include}
)
cmake_print_variables(LIBFIBER_INCLUDE_DIR)

include_directories(
        ${fiber_include}
        ${fiber_cpp_path}/src
        ${fiber_cpp_path}/include
        ${acl_include}
        ${master_include}
        ${acl_cpp_include}
)

find_library(LIBFIBER_LIBRARY NAMES libfiber.a PATHS ${CURRENT_LIB_PATH}/lib REQUIRED)
add_library(libfiber STATIC IMPORTED GLOBAL)
set_target_properties(libfiber PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${LIBFIBER_INCLUDE_DIR}"
        IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
        IMPORTED_LOCATION "${LIBFIBER_LIBRARY}"
        )


find_library(LIBFIBER_CPP_LIBRARY NAMES libfiber_cpp.a PATHS ${CURRENT_LIB_PATH}/lib REQUIRED)
add_library(libfiber_cpp STATIC IMPORTED GLOBAL)
set_target_properties(libfiber_cpp PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${LIBFIBER_INCLUDE_DIR}"
        IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
        IMPORTED_LOCATION "${LIBFIBER_CPP_LIBRARY}"
        )


find_library(LIBACL_LIBRARY NAMES libacl.a PATHS ${CURRENT_LIB_PATH}/lib REQUIRED)
add_library(libacl STATIC IMPORTED GLOBAL)
set_target_properties(libacl PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${LIBFIBER_INCLUDE_DIR}"
        IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
        IMPORTED_LOCATION "${LIBACL_LIBRARY}"
        )

find_library(LIBACL_CPP_LIBRARY NAMES libacl_cpp.a PATHS ${CURRENT_LIB_PATH}/lib REQUIRED)
add_library(libacl_cpp STATIC IMPORTED GLOBAL)
set_target_properties(libacl_cpp PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${LIBFIBER_INCLUDE_DIR}"
        IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
        IMPORTED_LOCATION "${LIBACL_CPP_LIBRARY}"
        )


find_library(LIBPROTOCOL_LIBRARY NAMES libprotocol.a PATHS ${CURRENT_LIB_PATH}/lib REQUIRED)
add_library(libprotocol STATIC IMPORTED GLOBAL)
set_target_properties(libprotocol PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${LIBFIBER_INCLUDE_DIR}"
        IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
        IMPORTED_LOCATION "${LIBPROTOCOL_LIBRARY}"
        )

find_library(LIBACL_FIBER_LIBRARY NAMES libacl_fiber.a PATHS ${CURRENT_LIB_PATH}/lib REQUIRED)
add_library(libacl_fiber STATIC IMPORTED GLOBAL)
set_target_properties(libacl_fiber PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${LIBFIBER_INCLUDE_DIR}"
        IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
        IMPORTED_LOCATION "${LIBACL_FIBER_LIBRARY}"
        )        
#// ar crsT libacl_fiber.a libacl.a libprotocol.a libacl_cpp.a libfiber.a libfiber_cpp.a