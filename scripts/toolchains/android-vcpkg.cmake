# android-vcpkg.cmake  (plain ASCII, minimal)

# Target platform (Gradle/AGP supplies the ABI & NDK)
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 24)           # your min API
set(CMAKE_ANDROID_STL_TYPE c++_shared)

# Gradle passes -D CMAKE_ANDROID_NDK; require it (no env fallback)
if(NOT DEFINED CMAKE_ANDROID_NDK OR NOT EXISTS "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")
  message(FATAL_ERROR "CMAKE_ANDROID_NDK not set or invalid: ${CMAKE_ANDROID_NDK}")
endif()

# vcpkg root must be passed from Gradle as -DVCPKG_ROOT=...
if(NOT DEFINED VCPKG_ROOT OR NOT EXISTS "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
  message(FATAL_ERROR "VCPKG_ROOT not set or invalid: ${VCPKG_ROOT}")
endif()

# vcpkg must chain-load the Android toolchain (cache so nested configs see it)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE
    "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake"
    CACHE PATH "" FORCE)

# Triplet: Android arm64 (override from Gradle if you like)
set(VCPKG_TARGET_TRIPLET "arm64-android" CACHE STRING "" FORCE)
set(VCPKG_CMAKE_SYSTEM_NAME "Android"    CACHE STRING "" FORCE)

# Use vcpkg as the primary toolchain
include("${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# (Optional) helpful logs
message(STATUS "Using Android NDK: ${CMAKE_ANDROID_NDK}")
message(STATUS "Using vcpkg root:  ${VCPKG_ROOT}")
