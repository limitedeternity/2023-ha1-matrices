get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(MAKE_C_IDENTIFIER ${PROJECT_NAME} PROJECT_NAME)

file(GLOB_RECURSE Header_Files
    LIST_DIRECTORIES OFF
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    "include/*.h" "include/*.hpp"
)

source_group("Header Files" FILES ${Header_Files})

file(GLOB_RECURSE Source_Files
    LIST_DIRECTORIES OFF
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    "src/*.c" "src/*.cpp"
)

source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

list(APPEND CMAKE_MESSAGE_INDENT "> ")

foreach(file IN LISTS ALL_FILES)
    message(STATUS "Located: ${file}")
endforeach()

list(REMOVE_AT CMAKE_MESSAGE_INDENT 0)

################################################################################

add_executable(${PROJECT_NAME} ${ALL_FILES})

################################################################################

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        /WX
        /EHsc
    )

    target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:CONSOLE)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Werror
        -fPIE
    )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
