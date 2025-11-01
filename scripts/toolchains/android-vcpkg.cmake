# android-vcpkg.cmake (unused stub)
# This project now uses vcpkg as the primary toolchain and chain-loads the
# official Android NDK toolchain from Gradle. Do not point CMAKE_TOOLCHAIN_FILE
# at this file anymore.

message(FATAL_ERROR
  "This project uses vcpkg.cmake as the toolchain.\n"
  "Use these Gradle arguments instead:\n"
  "  -DCMAKE_TOOLCHAIN_FILE=C:/projects/vcpkg/scripts/buildsystems/vcpkg.cmake\n"
  "  -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${android.ndkDirectory}/build/cmake/android.toolchain.cmake\n"
)
