# ======================================================================
# vcpkg + Android NDK toolchain (chain-loaded)  [arm64-v8a]
# ======================================================================

# Normalize & resolve the NDK path if AGP didn’t pass it
if(NOT DEFINED CMAKE_ANDROID_NDK OR CMAKE_ANDROID_NDK STREQUAL "")
    if(DEFINED ENV{ANDROID_NDK_HOME} AND NOT "$ENV{ANDROID_NDK_HOME}" STREQUAL "")
        set(CMAKE_ANDROID_NDK "$ENV{ANDROID_NDK_HOME}")
    elseif(DEFINED ENV{ANDROID_NDK_ROOT} AND NOT "$ENV{ANDROID_NDK_ROOT}" STREQUAL "")
        set(CMAKE_ANDROID_NDK "$ENV{ANDROID_NDK_ROOT}")
    endif()
endif()
if(DEFINED CMAKE_ANDROID_NDK AND NOT CMAKE_ANDROID_NDK STREQUAL "")
    file(TO_CMAKE_PATH "${CMAKE_ANDROID_NDK}" CMAKE_ANDROID_NDK)
endif()

# Basic Android settings (AGP also passes these; setting here is idempotent)
set(ANDROID_ABI "arm64-v8a" CACHE STRING "")
set(ANDROID_PLATFORM "android-24" CACHE STRING "")
set(CMAKE_ANDROID_STL_TYPE "c++_shared" CACHE STRING "")

# vcpkg root (prefer var, then env) and normalize
if(NOT DEFINED VCPKG_ROOT OR VCPKG_ROOT STREQUAL "")
    if(DEFINED ENV{VCPKG_ROOT} AND NOT "$ENV{VCPKG_ROOT}" STREQUAL "")
        set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
    endif()
endif()
if(DEFINED VCPKG_ROOT AND NOT VCPKG_ROOT STREQUAL "")
    file(TO_CMAKE_PATH "${VCPKG_ROOT}" VCPKG_ROOT)
endif()

# Tell vcpkg to chain-load the official Android toolchain
if(DEFINED CMAKE_ANDROID_NDK AND EXISTS "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")
    set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE
            "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake" CACHE FILEPATH "")
endif()

# Include vcpkg toolchain (this will load the NDK toolchain above)
if(DEFINED VCPKG_ROOT AND EXISTS "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    include("${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
else()
    message(FATAL_ERROR "vcpkg.cmake not found; set VCPKG_ROOT.")
endif()

# Diagnostics
message(STATUS "Using Android NDK: ${CMAKE_ANDROID_NDK}")
message(STATUS "Using vcpkg root:   ${VCPKG_ROOT}")
message(STATUS "ANDROID_ABI:        ${ANDROID_ABI}")
message(STATUS "ANDROID_PLATFORM:   ${ANDROID_PLATFORM}")
