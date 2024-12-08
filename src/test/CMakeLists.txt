project(VDSProject_test CXX C)
cmake_minimum_required(VERSION 3.10)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(VDSProject_test main_test.cpp)
target_link_libraries(VDSProject_test Manager)
target_link_libraries(VDSProject_test gtest gtest_main pthread)
target_link_libraries(VDSProject_test Manager gtest gtest_main pthread)
target_link_libraries(VDSProject_test Manager ${GTEST_LIBRARIES} pthread)
#target_link_libraries(tests ${GTEST_LIBRARIES} pthread)
