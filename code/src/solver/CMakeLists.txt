cmake_minimum_required(VERSION 3.20)
project(solver VERSION 1.0.0 LANGUAGES CXX)

# Guard Logic to Prevent re-adding the solver library
if (TARGET solver)
    return()  # If solver already exists, skip the rest of this script
endif()

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

# Include dependencies only if solver is built independently
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Add dependencies as subdirectories
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../math           ${CMAKE_BINARY_DIR}/math)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../mesh           ${CMAKE_BINARY_DIR}/mesh)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../utilities      ${CMAKE_BINARY_DIR}/utilities)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../boundary       ${CMAKE_BINARY_DIR}/boundary)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../postprocessing ${CMAKE_BINARY_DIR}/postprocessing)
endif()

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
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - # 

# Add the library target
add_library(solver STATIC ${LIB_SOURCES})

# Make sure any linking libraries can see its headers
target_include_directories(solver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link existing libraries
target_link_libraries(solver PUBLIC meshReader math utilities boundary postprocessing)

# Add an option to build tests
option(BUILD_TESTS "Build tests for solver library" ON)
if (BUILD_TESTS)
    # Find GTest package (Comment if FetchContent is used)
    find_package(GTest REQUIRED)

    # Get test source files
    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cc)
    add_executable(solverTest ${TEST_SOURCES})

    # Link the library and GTest to the test executable
    target_link_libraries(solverTest PRIVATE solver GTest::GTest GTest::Main)
    # Uncomment if FetchContent is used
    # target_link_libraries(solverTest PRIVATE solver gtest gtest_main)

    # Get absolute path to the test meshes
    set(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/Common)
    # Add the directory as a compile definition
    target_compile_definitions(solverTest PUBLIC COMMON_DIR="${COMMON_DIR}")

    # Get absolute path to the test meshes
    set(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/Common)
    # Add the directory as a compile definition
    target_compile_definitions(solverTest PUBLIC COMMON_DIR="${COMMON_DIR}")

    # Post-build step to run tests
    add_custom_command(TARGET solverTest POST_BUILD
        COMMAND solverTest
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

# Add an option to build only the library or tests
option(BUILD_LIBRARY_ONLY "Only build the library without tests" OFF)
if (BUILD_LIBRARY_ONLY)
    message(STATUS "Building library only (tests will be skipped).")
    set_target_properties(solverTest PROPERTIES EXCLUDE_FROM_ALL TRUE)
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
