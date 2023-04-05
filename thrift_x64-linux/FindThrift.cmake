set(CURRENT_LIB_PATH ${CMAKE_CURRENT_LIST_DIR})
cmake_print_variables(CURRENT_LIB_PATH)
set(THRIFT_INCLUDE_DIR ${CURRENT_LIB_PATH}/include/)

INCLUDE_DIRECTORIES(${THRIFT_INCLUDE_DIR})

add_library(thrift STATIC IMPORTED GLOBAL)
set_target_properties(thrift PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${THRIFT_INCLUDE_DIR}"
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THRIFT_INCLUDE_DIR}"
        IMPORTED_LOCATION "${CURRENT_LIB_PATH}/lib/libthrift.a")

# Create imported target THRIFT_main
add_library(thriftz STATIC IMPORTED GLOBAL)
set_target_properties(thriftz PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${THRIFT_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "Threads::Threads;zlib"
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${THRIFT_INCLUDE_DIR}"
        IMPORTED_LOCATION "${CURRENT_LIB_PATH}/lib/libthriftz.a")


find_program(THRIFT_COMPILER NAMES thrift PATHS ${CURRENT_LIB_PATH}/tools/thrift NO_DEFAULT_PATH REQUIRED)
cmake_print_variables(THRIFT_COMPILER)

if (THRIFT_COMPILER)
    exec_program(${THRIFT_COMPILER}
            ARGS -version OUTPUT_VARIABLE __thrift_OUT RETURN_VALUE THRIFT_RETURN)
    string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+-[a-z]+$" THRIFT_VERSION_STRING ${__thrift_OUT})

    # define utility function to generate cpp files
    function(thrift_gen_cpp thrift_file THRIFT_CPP_FILES_LIST THRIFT_GEN_INCLUDE_DIR)
        set(_res)
        set(_res_inc_path)
        get_filename_component(ABS_FIL ${thrift_file} ABSOLUTE)
        if(EXISTS ${ABS_FIL})
            get_filename_component(_target_dir ${thrift_file} NAME_WE)
            message("thrif_gen_cpp: ${thrift_file}")

            if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${_target_dir})
                file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_target_dir})
            endif()

            execute_process(
                    COMMAND ${THRIFT_COMPILER} -o "${CMAKE_CURRENT_BINARY_DIR}/${_target_dir}" --gen cpp:no_skeleton ${ABS_FIL}
                    OUTPUT_VARIABLE _stdout
                    ERROR_VARIABLE _stderr)
            message(STATUS "thrift generator output: ${_stdout}")
            if (NOT _stderr STREQUAL "")
                message(FATAL_ERROR "thrift generator output: ${_stderr}")
            endif()
            file(GLOB_RECURSE __result_src "${CMAKE_CURRENT_BINARY_DIR}/${_target_dir}/*.cpp")
            file(GLOB_RECURSE __result_hdr "${CMAKE_CURRENT_BINARY_DIR}/${_target_dir}/*.h")
            list(APPEND _res ${__result_src})
            list(APPEND _res ${__result_hdr})
            if(__result_hdr)
                list(GET __result_hdr 0 _res_inc_path)
                get_filename_component(_res_inc_path ${_res_inc_path} DIRECTORY)
            endif()
        else()
            message("thrift_gen_cpp: file ${thrift_file} does not exists")
        endif()
        set(${THRIFT_CPP_FILES_LIST} "${_res}" PARENT_SCOPE)
        set(${THRIFT_GEN_INCLUDE_DIR} "${_res_inc_path}" PARENT_SCOPE)
    endfunction()
endif ()
