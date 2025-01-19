cmake_minimum_required(VERSION 3.20)
project(MeshLib VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enforce static linking of the runtime for MSVC
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Add source files using wildcards for .cc and .hh files
file(GLOB LIB_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

# Specify include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
)

# Specify the binary directories for math
set(MATH_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../math/build")
# Add subdirectories with their own binary directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../math ${MATH_BINARY_DIR})

# Add the meshReader library target
add_library(meshReader STATIC ${LIB_SOURCES})

# Ensure any libraries linking to meshReader can see its headers
target_include_directories(meshReader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# Link meshReader library with math and utilities libraries, public to ensure meshReader propagates math's include directories
target_link_libraries(meshReader PUBLIC math)

# Optionally add the test target (if you are building tests)
option(BUILD_TESTS "Build tests for meshReader" ON)
if (BUILD_TESTS)
    # Find GTest package
    find_package(GTest REQUIRED)

    # Add test source files using wildcards for .cc files
    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/**/*.cc)

    # Add the test executable
    add_executable(meshReaderTest ${TEST_SOURCES})

    # Link the test executable to meshReader and GTest
    target_link_libraries(meshReaderTest PRIVATE meshReader GTest::GTest GTest::Main)

    # Move the test meshes to the build directory
    # file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/su2/meshes DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test/su2)

    # Get absolute path to the test meshes
    set(SU2_MESH_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/su2/meshes)
    # Add the directory as a compile definition
    target_compile_definitions(meshReaderTest PUBLIC SU2_MESH_DIR="${SU2_MESH_DIR}")

    # Post-build step to run tests
    add_custom_command(TARGET meshReaderTest POST_BUILD
        COMMAND meshReaderTest
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

# Optionally, to only build the library:
# cmake -S . -B build -DBUILD_TESTS=OFF

# Option to build everything (Library + Tests) or only the library
# Build everything (Library + Tests):
# cmake -S . -B build -DBUILD_TESTS=ON
# cmake --build build --config Release

# Build only the library:
# cmake -S . -B build -DBUILD_TESTS=OFF
# cmake --build build --config Release

# Run Tests (if built): Tests are automatically run after building when the BUILD_TESTS option is enabled.
# To run them separately:
# ./build/meshReaderTest