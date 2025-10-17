# ======================================================================
# MobyDick Android + vcpkg Toolchain (arm64-v8a)
# ======================================================================

# Platform/ABI
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 24)         # min API level you support
set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
set(CMAKE_ANDROID_STL_TYPE c++_shared)

# Prefer the NDK path Gradle/AGP passes on the command line.
# If not set (e.g., you run cmake manually), fall back to env vars.
if(NOT DEFINED CMAKE_ANDROID_NDK OR CMAKE_ANDROID_NDK STREQUAL "")
    if(DEFINED ENV{ANDROID_NDK_HOME} AND NOT "$ENV{ANDROID_NDK_HOME}" STREQUAL "")
        file(TO_CMAKE_PATH "$ENV{ANDROID_NDK_HOME}" CMAKE_ANDROID_NDK)
    elseif(DEFINED ENV{ANDROID_NDK_ROOT} AND NOT "$ENV{ANDROID_NDK_ROOT}" STREQUAL "")
        file(TO_CMAKE_PATH "$ENV{ANDROID_NDK_ROOT}" CMAKE_ANDROID_NDK)
    endif()
endif()

# Normalize to forward slashes if it exists (prevents backslash issues)
if(DEFINED CMAKE_ANDROID_NDK AND NOT CMAKE_ANDROID_NDK STREQUAL "")
    file(TO_CMAKE_PATH "${CMAKE_ANDROID_NDK}" CMAKE_ANDROID_NDK)
endif()

# vcpkg root: prefer normal var, then env var; normalize slashes
if(NOT DEFINED VCPKG_ROOT OR VCPKG_ROOT STREQUAL "")
    if(DEFINED ENV{VCPKG_ROOT} AND NOT "$ENV{VCPKG_ROOT}" STREQUAL "")
        set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
    endif()
endif()
if(DEFINED VCPKG_ROOT AND NOT VCPKG_ROOT STREQUAL "")
    file(TO_CMAKE_PATH "${VCPKG_ROOT}" VCPKG_ROOT)
endif()

# Integrate vcpkg (only if VCPKG_ROOT is known)
if(DEFINED VCPKG_ROOT AND EXISTS "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    include("${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
else()
    message(STATUS "vcpkg.cmake not found (VCPKG_ROOT='${VCPKG_ROOT}'). Continuing without vcpkg integration.")
endif()

# Diagnostics
message(STATUS "Using Android NDK: ${CMAKE_ANDROID_NDK}")
message(STATUS "Using vcpkg root:   ${VCPKG_ROOT}")
message(STATUS "Target ABI:         ${CMAKE_ANDROID_ARCH_ABI}")
message(STATUS "API Level:          ${CMAKE_SYSTEM_VERSION}")
