###### gtest & gmock
set(CURRENT_LIB_PATH ${CMAKE_CURRENT_LIST_DIR})
cmake_print_variables(CURRENT_LIB_PATH)
set(GTEST_INCLUDE_DIR ${CURRENT_LIB_PATH}/include/)

INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIR})

find_package(Threads REQUIRED)

add_library(gtest STATIC IMPORTED GLOBAL)
set_target_properties(gtest PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "Threads::Threads"
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
        IMPORTED_LOCATION "${CURRENT_LIB_PATH}/lib/libgtest.a"
        )

# Create imported target gtest_main
add_library(gtest_main STATIC IMPORTED GLOBAL)
set_target_properties(gtest_main PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "Threads::Threads;gtest"
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
        IMPORTED_LOCATION "${CURRENT_LIB_PATH}/lib/libgtest_main.a"

        )

# Create imported target gmock
add_library(gmock STATIC IMPORTED GLOBAL)
set_target_properties(gmock PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "Threads::Threads;gtest"
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
        IMPORTED_LOCATION "${CURRENT_LIB_PATH}/lib/libgmock.a"
        )
