cmake_minimum_required(VERSION 3.14)
project(tcp-to-aedat4-converter 
    VERSION 1.0.0 
    LANGUAGES CXX
    DESCRIPTION "Convert raw binary event camera frames over TCP to AEDAT4 format"
)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall")

# Find required packages
find_package(dv-processing REQUIRED)
find_package(OpenCV REQUIRED)

# Print found packages info
message(STATUS "Found dv-processing: ${dv-processing_VERSION}")
message(STATUS "Found OpenCV: ${OpenCV_VERSION}")

# Main converter executable
add_executable(converter
    src/main.cpp
    src/tcp_receiver.cpp
    src/frame_unpacker.cpp
)

# Include directories
target_include_directories(converter PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(converter PRIVATE
    dv::processing
    ${OpenCV_LIBS}
)

# Platform-specific settings
if(WIN32)
    # Windows socket library
    target_link_libraries(converter PRIVATE ws2_32)
endif()

if(UNIX AND NOT APPLE)
    # Linux - link pthread
    target_link_libraries(converter PRIVATE pthread)
endif()

# Install target (optional)
install(TARGETS converter DESTINATION bin)

# Print configuration summary
message(STATUS "")
message(STATUS "=== TCP to AEDAT4 Converter Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
