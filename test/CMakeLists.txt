add_definitions(-DBOOST_TEST_DYN_LINK)

include_directories(../src)

find_package(Boost 1.53.0 COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(test test.cpp)

target_link_libraries(test
    ${Boost_LIBRARIES})
