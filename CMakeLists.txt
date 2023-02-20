cmake_minimum_required(VERSION 3.2)
project(discord-presence LANGUAGES CXX)

# stuff which discord rpc apparently breaks without, so I copy pasted it here
find_file(RAPIDJSONTEST NAMES rapidjson rapidjson-1.1.0 PATHS ${PROJECT_SOURCE_DIR}/rpclib/thirdparty CMAKE_FIND_ROOT_PATH_BOTH)
if (NOT RAPIDJSONTEST)
    message("no rapidjson, download")
    set(RJ_TAR_FILE ${PROJECT_SOURCE_DIR}/rpclib/thirdparty/v1.1.0.tar.gz)
    file(DOWNLOAD https://github.com/miloyip/rapidjson/archive/v1.1.0.tar.gz ${RJ_TAR_FILE})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf ${RJ_TAR_FILE}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/rpclib/thirdparty
    )
    file(REMOVE ${RJ_TAR_FILE})
endif(NOT RAPIDJSONTEST)


set(BUILD_EXAMPLES OFF CACHE BOOL "Build example apps")
add_subdirectory(rpclib)

add_library(${PROJECT_NAME} SHARED "src/discord-presence.cpp")

target_link_libraries(${PROJECT_NAME} PUBLIC discord-rpc)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/rpclib/include>)
target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<$<C_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall>)
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    $<$<PLATFORM_ID:Windows>:_CRT_NONSTDC_NO_DEPRECATE>
    $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_DEPRECATE>
    $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    C_STANDARD 99
    PREFIX lib)

# This cannot be a generator expression in this version of CMake
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set_property(TARGET ${PROJECT_NAME} PROPERTY SUFFIX .so)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR})
