cmake_minimum_required(VERSION 3.26)

include(cmake/modules.cmake)
include(GitVersion)

project(commline
    VERSION ${GIT_VERSION_ABBREV}
    DESCRIPTION "A framework for creating command line applications in C++"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS NO)

include(ProjectTesting)
include(packages.cmake)

add_library(commline "")
add_library(commline::commline ALIAS commline)

target_sources(commline PUBLIC FILE_SET HEADERS BASE_DIRS include)
target_link_libraries(commline
    PUBLIC fmt::fmt
    PRIVATE ext::ext
)

if(PROJECT_TESTING)
    add_executable(commline.test "")

    target_link_libraries(commline.test
        PRIVATE
            commline
            GTest::gtest_main
    )

    add_test("Unit Tests" commline.test)
endif()

add_subdirectory(include)
add_subdirectory(src)

include(Sanitizers)
enable_asan()

include(EasyInstall)
easy_install(
    TARGETS
        commline
    DEPENDENCIES
        fmt
)

if(PROJECT_IS_TOP_LEVEL)
    add_custom_target(docker
        docker build --progress=plain --no-cache .
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        USES_TERMINAL
    )
endif()
