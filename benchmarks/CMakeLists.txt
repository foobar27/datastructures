add_definitions(-DBOOST_TEST_DYN_LINK)

include_directories(../src)

find_package(Boost 1.53.0 COMPONENTS timer system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(bench bench.cpp)

target_link_libraries(bench
    ${Boost_LIBRARIES})
