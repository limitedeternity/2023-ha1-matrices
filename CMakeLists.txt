cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

get_filename_component(PROJECT_NAME ${CMAKE_SOURCE_DIR} NAME)
string(MAKE_C_IDENTIFIER ${PROJECT_NAME} PROJECT_NAME)
project(${PROJECT_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Remove default flags provided by MSVC for CMake
if(MSVC)
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################

include_directories("${CMAKE_SOURCE_DIR}/src/common")

function(list_targets result)
    file(GLOB_RECURSE cmakelists
        LIST_DIRECTORIES OFF
        RELATIVE ${CMAKE_SOURCE_DIR}
        CONFIGURE_DEPENDS
        "src/*/CMakeLists.txt"
    )

    set(target_paths "")

    foreach(cmakelist IN LISTS cmakelists)
        get_filename_component(target_path ${cmakelist} DIRECTORY)
        list(APPEND target_paths ${target_path})
    endforeach()

    set(${result} ${target_paths} PARENT_SCOPE)
endfunction()

list_targets(target_paths)

foreach(target_path IN LISTS target_paths)
    message(STATUS "Adding target: ${target_path}")
    add_subdirectory(${target_path})
endforeach()
