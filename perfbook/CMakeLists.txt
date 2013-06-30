set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.53.0 COMPONENTS timer system program_options thread REQUIRED)
find_package(LibURCU REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${LIBURCU_INCLUDE_DIRS})

add_executable(counter-benchmark counter-benchmark.cpp)

target_link_libraries(counter-benchmark
  ${Boost_LIBRARIES}
  ${LIBURCU_LIBRARIES})
