cmake_minimum_required(VERSION 2.8)
project(xeno-test CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config")

find_package(Xenomai REQUIRED)

message("
=====================================================
Xenomai version ${XENOMAI_VERSION}

    Libraries : ${XENOMAI_LIBRARIES}
    Include directories : ${XENOMAI_INCLUDE_DIRS}

=====================================================
")

include_directories(${XENOMAI_INCLUDE_DIRS})
add_executable(xeno-test test.cpp)
target_link_libraries(xeno-test ${XENOMAI_LIBRARIES})
add_executable(xeno-test-cond test-cond.cpp)
target_link_libraries(xeno-test-cond ${XENOMAI_LIBRARIES})