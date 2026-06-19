cmake_minimum_required(VERSION 3.25)
project(Subnautica2Mods)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Path to UE4SS installation (override via -DUE4SS_DIR=...)
set(UE4SS_DIR "${CMAKE_SOURCE_DIR}/vendor/UE4SS" CACHE PATH "Path to UE4SS SDK root")

if(NOT EXISTS "${UE4SS_DIR}/include/Mod/CppUserModBase.hpp")
    message(WARNING "UE4SS headers not found at ${UE4SS_DIR}/include — C++ mods will not build.\nRun: scripts/fetch_ue4ss.bat to download them.")
endif()

# Shared compile options for all mods
add_library(mod_common_options INTERFACE)
target_include_directories(mod_common_options INTERFACE
    "${UE4SS_DIR}/include"
    "${UE4SS_DIR}/include/Unreal"
    "${CMAKE_SOURCE_DIR}/shared/include"
)
target_compile_options(mod_common_options INTERFACE
    /W4
    /WX-
    /permissive-
    /Zc:preprocessor
)
target_compile_definitions(mod_common_options INTERFACE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    _UNICODE
)

# Register all C++ mods here (add_subdirectory per mod)
add_subdirectory(mods/cpp/InfiniteHealth)
