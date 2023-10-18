cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

if(ENABLE_COVERAGE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(PACKAGE_VERSION 2.0)

# set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fopenmp -Wall -pthread")
if(ENABLE_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -coverage")
  set(ENABLE_UNITTEST True)
endif()
# if(ENABLE_LEAK)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fsanitize=address -fsanitize-recover=all  -fsanitize=leak")
# endif()

project(Geometry)

# find dependencies
find_package(Threads REQUIRED)
find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(OpenMP REQUIRED)


if(ENABLE_UNITTEST)
  find_package(GTest REQUIRED)
  enable_testing()
endif()

include_directories(
${EIGEN3_INCLUDE_DIRS}
${Boost_INCLUDE_DIRS}
${YAML_CPP_INCLUDE_DIR}
${GTEST_INCLUDE_DIRS}
)

## dependencies ##
set(dependencies 
${Boost_LIBRARIES}
${YAML_CPP_LIBRARIES}
${CMAKE_THREAD_LIBS_INIT}
${OpenMP_CXX_LIBRARIES}
)


set(source  ./src/vec2d.cc
            ./src/spin_mutex.cc
            ./src/polygon2d.cc
            ./src/region2d.cc
            ./src/multiple_polygon2d.cc
            # ./src/region_monitor.cc
)


add_library(lib_${PROJECT_NAME} SHARED ${source})
target_include_directories(lib_${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE src)
target_link_libraries(lib_${PROJECT_NAME} ${dependencies})


# add_executable(justtest ./test/justtest.cc)
# target_include_directories(justtest PUBLIC
#   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#   $<INSTALL_INTERFACE:include>
#   PRIVATE src)


message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")


## Unit test ##
if(ENABLE_UNITTEST)
  add_executable(vec2d_test     ./test/vec2d_test.cc)
  target_link_libraries(vec2d_test ${dependencies} lib_${PROJECT_NAME} gtest gtest_main)

  add_executable(spin_mutex_test     ./test/spin_mutex_test.cc)
  target_link_libraries(spin_mutex_test ${dependencies} lib_${PROJECT_NAME} gtest gtest_main)

  add_executable(polygon2d_test    ./test/polygon2d_test.cc)
  target_link_libraries(polygon2d_test ${dependencies} lib_${PROJECT_NAME} gtest gtest_main)

  add_executable(multiple_polygon2d_test    ./test/multiple_polygon2d_test.cc)
  target_link_libraries(multiple_polygon2d_test ${dependencies} lib_${PROJECT_NAME} gtest gtest_main)

  gtest_discover_tests(vec2d_test)
  gtest_discover_tests(spin_mutex_test)
  gtest_discover_tests(polygon2d_test)
  gtest_discover_tests(multiple_polygon2d_test)
endif()
