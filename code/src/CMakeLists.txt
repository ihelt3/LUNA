cmake_minimum_required(VERSION 3.20)
project(LUNA VERSION 1.0.0 LANGUAGES CXX)

# Set global C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enforce static linking of the runtime for MSVC
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Set optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")

# Options
option(BUILD_TESTS "Build all tests" ON)
option(BUILD_LIBRARY_ONLY "Only build the libraries without tests" OFF)

# Add subdirectories (each should have its own CMakeLists.txt)
add_subdirectory(boundary)
add_subdirectory(math)
add_subdirectory(mesh)
# add_subdirectory(physics)
add_subdirectory(postprocessing)
add_subdirectory(solver)
add_subdirectory(utilities)

# If testing is enabled, make sure the required dependencies are available
if (BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    # Link test executables to GTest and other dependencies
    add_test(NAME MathTests COMMAND mathTest)
    add_test(NAME BoundaryTests COMMAND boundaryTest)
    add_test(NAME MeshTests COMMAND meshReaderTest)
    add_test(NAME SolverTests COMMAND solverTest)
    add_test(NAME UtilitiesTests COMMAND utilitiesTest)
    add_test(NAME PostprocessingTests COMMAND postprocessingTest)
    # Add more test executables if needed
endif()

# Optionally, exclude tests from the default build
if (BUILD_LIBRARY_ONLY)
    message(STATUS "Building only libraries, skipping tests.")
    set_target_properties(mathTest boundaryTest PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()