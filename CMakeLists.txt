cmake_minimum_required(VERSION 3.8.0)

set(PATCH_VERSION 1 CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(task06_matrix VERSION ${PROJECT_VERSION})

set(PROJECT_NAME_TEST ${PROJECT_NAME}_test)
set(SRC_H Matrix.h)
set(SRC_CPP)
set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 17)

# cli
add_executable(${PROJECT_NAME} ${SRC_H} ${SRC_CPP} main.cpp)

# test
include (FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
set(SRC_H_TEST ${SRC_H} Matrix_test.h)
add_executable(${PROJECT_NAME_TEST} ${SRC_H_TEST} main_test.cpp)
target_link_libraries(${PROJECT_NAME_TEST} GTest::gtest_main)
include (GoogleTest)
gtest_discover_tests(${PROJECT_NAME_TEST})

# deb
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)
include (CPack)
