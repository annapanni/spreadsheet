cmake_minimum_required(VERSION 3.25)

project(spreadsheet)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


function(add_compile_options target)
    target_compile_options(${target} PRIVATE
            # Ezeket kapcsold be!:
            #            -Werror
            #            -Wall
            #            -Wextra
            #            -Wpedantic
            #            -Wconversion
    )
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PRIVATE
                -fsanitize=address
                -fno-omit-frame-pointer
        )
        target_link_options(${target} PRIVATE
                -fsanitize=address
        )
    endif ()
endfunction()


add_library(${PROJECT_NAME}_lib)
add_compile_options(${PROJECT_NAME}_lib)
target_include_directories(${PROJECT_NAME}_lib PUBLIC srcs)
target_sources(${PROJECT_NAME}_lib PRIVATE
        srcs/console.cpp
        srcs/expression.cpp
        srcs/parser.cpp
        srcs/sheet.cpp
        srcs/token.cpp
)


add_executable(${PROJECT_NAME})
add_compile_options(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
        srcs/main.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)


enable_testing()

add_executable(${PROJECT_NAME}_test)
add_compile_options(${PROJECT_NAME}_test)
target_sources(${PROJECT_NAME}_test PRIVATE
        srcs/test.cpp
)
target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME}_lib)

find_package(GTest QUIET)
if (NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
endif ()
target_link_libraries(${PROJECT_NAME}_test PRIVATE
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)
