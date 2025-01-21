cmake_minimum_required(VERSION 3.20)
project(math VERSION 1.0.0 LANGUAGES CXX)

# Guard Logic to Prevent re-adding the math library
if (TARGET math)
    return()  # If math already exists, skip the rest of this script
endif()

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enforce static linking of the runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Set optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")

# Add an option to build tests
option(BUILD_TESTS "Build tests for math" ON)

# Add source files automatically based on their extension
file(GLOB LIB_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

# Specify the include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

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
add_library(math STATIC ${LIB_SOURCES})

# Ensure any libraries linking to math can see its headers
target_include_directories(math PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Optionally add the test target
if (BUILD_TESTS)
    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc)
    add_executable(mathTest ${TEST_SOURCES})

    # Link the library and GTest to the test executable
    target_link_libraries(mathTest PRIVATE math GTest::GTest GTest::Main)
    # Uncomment if FetchContent is used
    # target_link_libraries(mathTest PRIVATE math gtest gtest_main)

    # Post-build step to run tests
    add_custom_command(TARGET mathTest POST_BUILD
        COMMAND mathTest
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Add an option to build only the library or tests
option(BUILD_LIBRARY_ONLY "Only build the library without tests" OFF)
if (BUILD_LIBRARY_ONLY)
    message(STATUS "Building library only (tests will be skipped).")
    set_target_properties(mathTest PROPERTIES EXCLUDE_FROM_ALL TRUE)
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
