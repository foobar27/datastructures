add_definitions(-DBOOST_TEST_DYN_LINK)

find_package(Boost 1.53.0 COMPONENTS timer system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(PegSolitaire PegSolitaire.cpp)

target_link_libraries(PegSolitaire
    ${Boost_LIBRARIES})
