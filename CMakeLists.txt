cmake_minimum_required(VERSION 3.3)
project(ovdbcsg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(AFTER SYSTEM /gitosis/openvdb-boolean/)
include_directories(AFTER SYSTEM /gitosis/boost_1_53_0/)
include_directories(AFTER SYSTEM /gitosis/movision/INCLUDE/tbb40_297oss/include/)

link_directories(/gitosis/openvdb-boolean/openvdb/)
link_directories(/gitosis/movision/INCLUDE/tbb40_297oss/build/macos_intel64_gcc_cc_os10_11_4_release/)

set(SOURCE_FILES main.cpp)
add_executable(ovdbcsg ${SOURCE_FILES})
target_link_libraries(ovdbcsg openvdb.3.1.0 tbb half)
