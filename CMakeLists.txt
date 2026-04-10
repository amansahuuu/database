cmake_minimum_required(VERSION 3.10)
project("catEaredNaruto")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatically find all source files
# Note: Run CMake again if you add new files
file(GLOB_RECURSE SOURCES "catEaredNaruto/src/*.cpp")
file(GLOB_RECURSE HEADERS "catEaredNaruto/src/*.h")

# Create the executable
add_executable(catEaredNaruto ${SOURCES} ${HEADERS} "catEaredNaruto/src/TheradPool/ThreadPool.h" "catEaredNaruto/src/TheradPool/ThreadPool.cpp" "catEaredNaruto/src/helpers/helper.cpp" "catEaredNaruto/src/helpers/helper.h" "catEaredNaruto/src/Welcome/welcome.h" "catEaredNaruto/src/Welcome/welcome.cpp" "catEaredNaruto/src/WAL/WAL.h" "catEaredNaruto/src/WAL/WAL.cpp" "catEaredNaruto/src/types/types.h" "catEaredNaruto/src/TTL/TTL.h")

# Include directories
target_include_directories(catEaredNaruto PRIVATE catEaredNaruto/src)

# Optional: Add compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(catEaredNaruto PRIVATE -Wall -Wextra)
endif()