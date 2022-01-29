cmake_minimum_required(VERSION 3.5)

project(Value LANGUAGES CXX C)

option(USE_GMP_LIB "" ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules)

include_directories(..)

if (${CMAKE_SYSTEM_NAME} MATCHES Darwin)
    add_link_options(-dead_strip -dead_strip_dylibs -Os)
endif ()
if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
    add_link_options(-Wl,--gc-sections)
    add_compile_options(-fdata-sections -ffunction-sections)
endif ()

if (USE_GMP_LIB)
    find_package(GMPXX REQUIRED)
    find_package(GMP REQUIRED)
    INCLUDE_DIRECTORIES("${GMPXX_INCLUDE_DIR}")
    link_libraries(${GMPXX_LIBRARIES})
    link_libraries(${GMP_LIBRARIES})
    add_executable(basic ${libsources} basic.cpp)
else()
    include_directories(BigNumber/src/BigNumber)
    add_executable(basic ${libsources} BigNumber/src/BigNumber/number.c BigNumber/src/BigNumber/BigNumber.cpp basic.cpp)
    add_definitions(-DUSE_BIG_NUMBER)
endif()

