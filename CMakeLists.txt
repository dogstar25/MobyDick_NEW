cmake_minimum_required(VERSION 3.21)
cmake_policy(VERSION 3.21)

project(MobyDickEngine VERSION 0.1.0 LANGUAGES CXX)

# Require C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =======================================================================
# Output directories (organized by configuration)
# =======================================================================
# All binaries (.exe, .dll) will go here
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../build/${CMAKE_BUILD_TYPE}/bin)
# All static and shared libraries (.lib, .dll, .so)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../build/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../build/${CMAKE_BUILD_TYPE}/lib)

# Make sure multi-config generators (like VS) also get unique folders
foreach(OUTPUTCONFIG IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_SOURCE_DIR}/../build/${OUTPUTCONFIG}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_SOURCE_DIR}/../build/${OUTPUTCONFIG}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} ${CMAKE_SOURCE_DIR}/../build/${OUTPUTCONFIG}/lib)
endforeach()

# Basic platform defines (optional)
if(WIN32)
    add_compile_definitions(MOBYDICK_PLATFORM_WINDOWS)
elseif(ANDROID)
    add_compile_definitions(MOBYDICK_PLATFORM_ANDROID)
elseif(APPLE)
    add_compile_definitions(MOBYDICK_PLATFORM_IOS)
endif()

# Just print a message to confirm config works
message(STATUS "Configured MobyDickEngine v${PROJECT_VERSION}")

# Collect all source files
file(GLOB_RECURSE MOBYDICK_SOURCES
    "src/*.cpp"
    "include/*.h"
    "include/*.hpp"
)

# Create the static library
add_library(MobyDickEngine STATIC ${MOBYDICK_SOURCES})

# Include directory so that other projects can do: #include <MobyDick.h>
target_include_directories(MobyDickEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# --- Dependencies via vcpkg ---
find_package(SDL2 CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(enkiTS CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)

target_link_libraries(MobyDickEngine
    PUBLIC
        box2d::box2d
        enkiTS::enkiTS
        glad::glad
        glm::glm
        JsonCpp::JsonCpp
        imgui::imgui
        SDL2::SDL2
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
)
