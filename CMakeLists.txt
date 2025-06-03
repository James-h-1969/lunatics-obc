# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(RpiProgram VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# Include user-defined headers
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Find required packages
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# Main executable
add_executable(rpi_program ${SOURCES})

# Link libraries manually
target_link_libraries(rpi_program PRIVATE
    pigpio
    pthread
    ${CURL_LIBRARIES}
    Threads::Threads
)

# Set output directory
set_target_properties(rpi_program PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Install target
install(TARGETS rpi_program DESTINATION bin)

# Custom targets
add_custom_target(run
    COMMAND rpi_program
    DEPENDS rpi_program
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMENT "Running the program"
)

add_custom_target(clean-build
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning build directory"
)

# Debug info
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
