cmake_minimum_required(VERSION 3.20)
project(utilitiesLib VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enforce static linking of the runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Set optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")

# Add source files automatically based on their extension
file(GLOB LIB_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

# Specify include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
)

# # Specify the binary directories for mesh reader
# message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
set(MESH_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../mesh/build")
# set(UTILITIES_BINARY_DIR "${CMAKE_SOURCE_DIR}/build")  # New binary directory for utilities

# # Add subdirectories with their own binary directories
add_subdirectory(${CMAKE_SOURCE_DIR}/../mesh ${MESH_BINARY_DIR})

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - # 
# FetchContent can be enabled if you want to use git to download dependencies

# # Use FetchContent to download dependencies (NOTE: git must be installed on your system to use this)
# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   GIT_REPOSITORY https://github.com/google/googletest.git
#   GIT_TAG release-1.11.0
# )
# FetchContent_MakeAvailable(googletest)

# Find GTest package (Comment if FetchContent is used)
find_package(GTest REQUIRED)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - # 

# Add the library target
add_library(utilitiesLib STATIC ${LIB_SOURCES})

# Link utilitiesLib library with meshReader library, public to ensure that utilitiesLib propagates meshReader's include directories
target_link_libraries(utilitiesLib PUBLIC meshReader)

# Add an option to build tests
option(BUILD_TESTS "Build tests for utilitiesLib" ON)
if (BUILD_TESTS)
    # Get test source files
    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/**/*.cc)
    add_executable(utilitiesTest ${TEST_SOURCES})

    # Link the library and GTest to the test executable
    target_link_libraries(utilitiesTest PRIVATE utilitiesLib GTest::GTest GTest::Main)
    # Uncomment if FetchContent is used
    # target_link_libraries(utilitiesTest PRIVATE utilitiesLib gtest gtest_main)

    # Get absolute path to the test meshes
    set(MESH_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/Common)
    # Add the directory as a compile definition
    target_compile_definitions(utilitiesTest PUBLIC MESH_DIR="${MESH_DIR}")
    
    # Post-build step to run tests
    add_custom_command(TARGET utilitiesTest POST_BUILD
        COMMAND utilitiesTest
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

# Add an option to build only the library or tests
option(BUILD_LIBRARY_ONLY "Only build the library without tests" OFF)
if (BUILD_LIBRARY_ONLY)
    message(STATUS "Building library only (tests will be skipped).")
    set_target_properties(utilitiesTest PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()

### Build Everything (Library + Tests):
# sh
# Copy
# cmake -S . -B build -DBUILD_TESTS=ON
# cmake --build build --config Release

### Build Only the Library:
# sh
# Copy
# cmake -S . -B build -DBUILD_TESTS=OFF
# cmake --build build --config Release


#### Run Tests (if built): Tests are automatically run after building when the BUILD_TESTS option is enabled. To run them separately:
# sh
# Copy
# ./build/mathTest
