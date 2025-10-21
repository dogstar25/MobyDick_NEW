# android-vcpkg.cmake — vcpkg primary; chain-loads official NDK toolchain

# 1) Target Android
set(CMAKE_SYSTEM_NAME Android)

# 2) Normalize NDK path provided by Gradle/AGP
if(NOT DEFINED CMAKE_ANDROID_NDK OR CMAKE_ANDROID_NDK STREQUAL "")
  if(DEFINED ANDROID_NDK AND NOT ANDROID_NDK STREQUAL "")
    set(CMAKE_ANDROID_NDK "${ANDROID_NDK}")
  elseif(DEFINED ANDROID_NDK_HOME AND NOT ANDROID_NDK_HOME STREQUAL "")
    set(CMAKE_ANDROID_NDK "${ANDROID_NDK_HOME}")
  elseif(DEFINED ANDROID_NDK_ROOT AND NOT ANDROID_NDK_ROOT STREQUAL "")
    set(CMAKE_ANDROID_NDK "${ANDROID_NDK_ROOT}")
  endif()
endif()
if(NOT DEFINED CMAKE_ANDROID_NDK OR NOT EXISTS "${CMAKE_ANDROID_NDK}")
  message(FATAL_ERROR "CMAKE_ANDROID_NDK not set or invalid")
endif()
file(TO_CMAKE_PATH "${CMAKE_ANDROID_NDK}" CMAKE_ANDROID_NDK)

# 2b) Make current configure AND vcpkg's child configures see the NDK
set(ANDROID_NDK       "${CMAKE_ANDROID_NDK}" CACHE PATH "Android NDK path" FORCE)
set(ANDROID_NDK_HOME  "${CMAKE_ANDROID_NDK}" CACHE PATH "Android NDK home" FORCE)
set(ANDROID_NDK_ROOT  "${CMAKE_ANDROID_NDK}" CACHE PATH "Android NDK root" FORCE)
# Export to env so child processes inherit
set(ENV{ANDROID_NDK}       "${CMAKE_ANDROID_NDK}")
set(ENV{ANDROID_NDK_HOME}  "${CMAKE_ANDROID_NDK}")
set(ENV{ANDROID_NDK_ROOT}  "${CMAKE_ANDROID_NDK}")

# 2c) Tell vcpkg to KEEP these env vars when it cleans the environment  <<<< NEW
set(VCPKG_KEEP_ENV_VARS "ANDROID_NDK;ANDROID_NDK_HOME;ANDROID_NDK_ROOT" CACHE STRING "" FORCE)
# (Older vcpkg used this name; harmless to set both)
set(VCPKG_ENV_PASSTHROUGH "ANDROID_NDK;ANDROID_NDK_HOME;ANDROID_NDK_ROOT" CACHE STRING "" FORCE)

# Also predefine ABI/platform so detect_compiler doesn’t guess
if(NOT DEFINED ANDROID_ABI)
  set(ANDROID_ABI "arm64-v8a" CACHE STRING "" FORCE)
endif()
if(NOT DEFINED ANDROID_PLATFORM)
  set(ANDROID_PLATFORM "android-24" CACHE STRING "" FORCE)
endif()

# 3) Chain-load the official Android toolchain
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE
    "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")

# 4) vcpkg root + triplet (override-able from Gradle args)
if(NOT DEFINED VCPKG_ROOT)
  set(VCPKG_ROOT "C:/projects/vcpkg")
endif()
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "arm64-android")
endif()
set(VCPKG_FEATURE_FLAGS "manifests")

# 5) Include vcpkg buildsystem (triggers detect_compiler)
include("${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# 6) Diagnostics
message(STATUS "Using Android NDK: ${CMAKE_ANDROID_NDK}")
message(STATUS "Using vcpkg root:  ${VCPKG_ROOT}")
message(STATUS "ANDROID_ABI:       ${ANDROID_ABI}")
message(STATUS "ANDROID_PLATFORM:  ${ANDROID_PLATFORM}")
message(STATUS "VCPKG_TRIPLET:     ${VCPKG_TARGET_TRIPLET}")
