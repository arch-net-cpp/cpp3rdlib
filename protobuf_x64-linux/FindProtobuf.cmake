
function(_protobuf_generate_cpp SRCS HDRS)
    if(NOT ARGN)
        message(SEND_ERROR "Error: _protobuf_generate_cpp() called without any proto files")
        return()
    endif()

    set(${SRCS})
    set(${HDRS})

    foreach(FIL ${ARGN})
        get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
        get_filename_component(FIL_WE ${FIL} NAME_WE)
        get_filename_component(PROTO_FILE_DIRECTORY ${FIL} DIRECTORY)


        set(_protobuf_protoc_src "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc")
        set(_protobuf_protoc_hdr "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.h")
        list(APPEND ${SRCS} "${_protobuf_protoc_src}")
        list(APPEND ${HDRS} "${_protobuf_protoc_hdr}")


        execute_process(
                COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}"
                COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
                    -I "${CMAKE_CURRENT_SOURCE_DIR}/${PROTO_FILE_DIRECTORY}"
                    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}" ${ABS_FIL}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                OUTPUT_VARIABLE _stdout
                ERROR_VARIABLE _stderr)

    endforeach()

    message(STATUS "protoc output: ${_stdout}")
    if (NOT _stderr STREQUAL "")
        message(FATAL_ERROR "protoc output: ${_stderr}")
    endif()

    set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
    set(${SRCS} ${${SRCS}} PARENT_SCOPE)
    set(${HDRS} ${${HDRS}} PARENT_SCOPE)
    message(STATUS "Generate Protobuf CPP Source Files: ${${SRCS}}")
    message(STATUS "Generate Protobuf CPP Header Files: ${${HDRS}}")
endfunction()


# Print and set the protobuf library information,
# finish this cmake process and exit from this file.
macro(PROMPT_PROTOBUF_LIB)
    SET(protobuf_DEPS ${ARGN})

    MESSAGE(STATUS "Google/protobuf executable: ${PROTOBUF_PROTOC_EXECUTABLE}")
    MESSAGE(STATUS "Google/protobuf library: ${PROTOBUF_LIBRARY}")
    MESSAGE(STATUS "Google/protobuf version: ${PROTOBUF_VERSION}")
    INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})

    # Assuming that all the protobuf libraries are of the same type.
    IF(${PROTOBUF_LIBRARY} MATCHES "${CMAKE_STATIC_LIBRARY_SUFFIX}$")
        SET(protobuf_LIBTYPE STATIC)
    ELSEIF(${PROTOBUF_LIBRARY} MATCHES "${CMAKE_SHARED_LIBRARY_SUFFIX}$")
        SET(protobuf_LIBTYPE SHARED)
    ELSE()
        MESSAGE(FATAL_ERROR "Unknown library type: ${PROTOBUF_LIBRARY}")
    ENDIF()

    ADD_LIBRARY(protobuf ${protobuf_LIBTYPE} IMPORTED GLOBAL)
    SET_TARGET_PROPERTIES(protobuf PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${PROTOBUF_INCLUDE_DIR}"
            IMPORTED_LOCATION "${PROTOBUF_LIBRARY}"
            INTERFACE_LINK_LIBRARIES "zlib"
            )

    ADD_LIBRARY(protobuf_lite ${protobuf_LIBTYPE} IMPORTED GLOBAL)
    SET_TARGET_PROPERTIES(protobuf_lite PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${PROTOBUF_INCLUDE_DIR}"
            IMPORTED_LOCATION "${PROTOBUF_LITE_LIBRARY}"
            INTERFACE_LINK_LIBRARIES "zlib"
            )

    ADD_LIBRARY(libprotoc ${protobuf_LIBTYPE} IMPORTED GLOBAL)
    SET_TARGET_PROPERTIES(libprotoc PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${PROTOBUF_INCLUDE_DIR}"
            IMPORTED_LOCATION "${PROTOC_LIBRARY}"
            INTERFACE_LINK_LIBRARIES "zlib"
            )

    ADD_EXECUTABLE(protoc IMPORTED GLOBAL)
    SET_PROPERTY(TARGET protoc PROPERTY IMPORTED_LOCATION ${PROTOBUF_PROTOC_EXECUTABLE})
    # FIND_Protobuf.cmake uses `Protobuf_PROTOC_EXECUTABLE`.
    # make `protobuf_generate_cpp` happy.
    SET(Protobuf_PROTOC_EXECUTABLE ${PROTOBUF_PROTOC_EXECUTABLE})

    FOREACH(dep ${protobuf_DEPS})
        ADD_DEPENDENCIES(protobuf ${dep})
        ADD_DEPENDENCIES(protobuf_lite ${dep})
        ADD_DEPENDENCIES(libprotoc ${dep})
        ADD_DEPENDENCIES(protoc ${dep})
    ENDFOREACH()

    RETURN()
endmacro()

macro(SET_PROTOBUF_VERSION)
    EXEC_PROGRAM(${PROTOBUF_PROTOC_EXECUTABLE} ARGS --version OUTPUT_VARIABLE PROTOBUF_VERSION)
    STRING(REGEX MATCH "[0-9]+.[0-9]+" PROTOBUF_VERSION "${PROTOBUF_VERSION}")
endmacro()


set(CURRENT_LIB_PATH ${CMAKE_CURRENT_LIST_DIR})
set(PROTOBUF_ROOT ${CURRENT_LIB_PATH})

find_path(PROTOBUF_INCLUDE_DIR google/protobuf/message.h PATHS ${PROTOBUF_ROOT}/include NO_DEFAULT_PATH REQUIRED)
find_library(PROTOBUF_LIBRARY protobuf PATHS ${PROTOBUF_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PROTOBUF_LITE_LIBRARY protobuf-lite PATHS ${PROTOBUF_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_library(PROTOBUF_PROTOC_LIBRARY protoc PATHS ${PROTOBUF_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
find_program(PROTOBUF_PROTOC_EXECUTABLE protoc PATHS ${PROTOBUF_ROOT}/tools/protobuf NO_DEFAULT_PATH REQUIRED)

cmake_print_variables(PROTOBUF_PROTOC_EXECUTABLE)

if (PROTOBUF_INCLUDE_DIR AND PROTOBUF_LIBRARY AND PROTOBUF_LITE_LIBRARY AND PROTOBUF_PROTOC_LIBRARY AND PROTOBUF_PROTOC_EXECUTABLE)
    message(STATUS "Using custom protobuf library in ${PROTOBUF_ROOT}.")
    SET_PROTOBUF_VERSION()
    PROMPT_PROTOBUF_LIB()
else()
    message(WARNING "Cannot find protobuf library in ${PROTOBUF_ROOT}.")
endif()