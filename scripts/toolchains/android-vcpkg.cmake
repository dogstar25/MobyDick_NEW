# android-vcpkg.cmake

# --- Android target basics ---
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 24)
set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
set(CMAKE_ANDROID_STL_TYPE c++_shared)

# Prefer Gradle-provided NDK; fallback to env
if(NOT DEFINED CMAKE_ANDROID_NDK OR CMAKE_ANDROID_NDK STREQUAL "")
  if(DEFINED ENV{ANDROID_NDK_HOME} AND NOT "$ENV{ANDROID_NDK_HOME}" STREQUAL "")
    file(TO_CMAKE_PATH "$ENV{ANDROID_NDK_HOME}" CMAKE_ANDROID_NDK)
  elseif(DEFINED ENV{ANDROID_NDK_ROOT} AND NOT "$ENV{ANDROID_NDK_ROOT}" STREQUAL "")
    file(TO_CMAKE_PATH "$ENV{ANDROID_NDK_ROOT}" CMAKE_ANDROID_NDK)
  endif()
endif()
# DO NOT include the NDK toolchain directly
# include("${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")  # <-- keep disabled

# vcpkg root
if(NOT DEFINED VCPKG_ROOT AND DEFINED ENV{VCPKG_ROOT})
  set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
endif()
file(TO_CMAKE_PATH "${VCPKG_ROOT}" VCPKG_ROOT)

# Tell vcpkg to chain-load the Android toolchain for all nested config runs
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE
    "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake"
    CACHE PATH "Chainloaded Android toolchain for vcpkg" FORCE)

# Ensure vcpkg knows we’re cross-building Android
set(VCPKG_TARGET_TRIPLET "arm64-android" CACHE STRING "" FORCE)
set(VCPKG_CMAKE_SYSTEM_NAME "Android"     CACHE STRING "" FORCE)

# Use vcpkg as the primary toolchain
include("${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# (optional) diagnostics
message(STATUS "Using Android NDK: ${CMAKE_ANDROID_NDK}")
message(STATUS "Using vcpkg root:   ${VCPKG_ROOT}")
message(STATUS "Target ABI:         ${CMAKE_ANDROID_ARCH_ABI}")
message(STATUS "API Level:          ${CMAKE_SYSTEM_VERSION}")
