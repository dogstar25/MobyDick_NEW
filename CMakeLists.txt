cmake_minimum_required(VERSION 3.22)

project(MobyDick VERSION 0.1.0 LANGUAGES CXX)

message(STATUS "?? MobyDick CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}")


# --- Global Settings ---
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#=========================================================================================
# Check if a built-in libiconv is available for the current API level
if (ANDROID_PLATFORM_API GREATER_EQUAL 28)
    # Link directly to the NDK's prebuilt libiconv
    add_library(iconv_lib SHARED IMPORTED)
    set_target_properties(iconv_lib PROPERTIES IMPORTED_LOCATION ${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/${ANDROID_ARCH_NAME}/libiconv.so)
else()
    # Fallback for older API levels, e.g., print a warning or handle differently
    message(WARNING "libiconv is not available for API Level ${ANDROID_PLATFORM_API}. Use vcpkg or update your minSdkVersion.")
endif()
#===================================================================================================================

# Enable MSVC multi-processor compilation
if (MSVC)
    add_compile_options(/MP)
endif()

# --- Source Groups ---
file(GLOB_RECURSE MOBYDICK_SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
)
file(GLOB_RECURSE MOBYDICK_HEADERS CONFIGURE_DEPENDS
    "include/*.h"
    "include/*.hpp"
)

# --- Library Target ---
add_library(MobyDick STATIC
    ${MOBYDICK_SOURCES}
    ${MOBYDICK_HEADERS}
)

set_target_properties(MobyDick PROPERTIES
    OUTPUT_NAME "MobyDick"
    PROJECT_LABEL "MobyDick (Engine Library)"
)

target_include_directories(MobyDick
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ---------------------------------------------------------
# Dependencies (resolved via vcpkg)
# ---------------------------------------------------------

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(enkiTS CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(Iconv REQUIRED)

# --- Link MobyDick cleanly per-platform ---
if (WIN32)
    target_link_libraries(MobyDick PUBLIC
            SDL2::SDL2
            SDL2::SDL2main
            SDL2_image::SDL2_image
            SDL2_mixer::SDL2_mixer
            SDL2_ttf::SDL2_ttf
            box2d::box2d
            enkiTS::enkiTS
            imgui::imgui
            glm::glm
            glad::glad
            JsonCpp::JsonCpp
            Iconv::Iconv
    )
elseif (ANDROID)
    # Your arm64-android triplet exports only the -static satellites:
    #   SDL2_image::SDL2_image-static, SDL2_mixer::SDL2_mixer-static, SDL2_ttf::SDL2_ttf-static
    # SDL2main is not used on Android.
    target_link_libraries(MobyDick PUBLIC
            SDL2::SDL2
            SDL2_image::SDL2_image-static
            SDL2_mixer::SDL2_mixer-static
            SDL2_ttf::SDL2_ttf-static
            box2d::box2d
            enkiTS::enkiTS
            imgui::imgui
            glm::glm
            glad::glad
            JsonCpp::JsonCpp
            iconv_lib
    )

else()
    message(FATAL_ERROR "Unsupported platform for MobyDick (expected WIN32 or ANDROID).")
endif()

# --- Platform Defines ---
if(WIN32)
    target_compile_definitions(MobyDick PUBLIC MOBYDICK_PLATFORM_WINDOWS)
elseif(ANDROID)
    target_compile_definitions(MobyDick PUBLIC MOBYDICK_PLATFORM_ANDROID)
elseif(IOS)
    target_compile_definitions(MobyDick PUBLIC MOBYDICK_PLATFORM_IOS)
endif()

# -------define the assets directory
#target_compile_definitions(MobyDick PUBLIC GAME_WORKING_DIR=\"${GAME_WORKING_DIR}\")


# --- Output Info ---
message(STATUS "Building MobyDick ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
