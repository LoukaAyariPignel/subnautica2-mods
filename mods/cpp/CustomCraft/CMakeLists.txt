project(CustomCraft VERSION 1.0.0)

# SDK Dumper-7 pour Subnautica 2.
# Source : https://github.com/Evasion3356/Subnautica-2-Cheat
# Cloner ce repo dans vendor/SN2SDK/ :
#   git submodule add https://github.com/Evasion3356/Subnautica-2-Cheat vendor/SN2SDK
set(SN2_SDK_DIR "${CMAKE_SOURCE_DIR}/vendor/SN2SDK" CACHE PATH "Dumper-7 SDK path")

if(NOT EXISTS "${SN2_SDK_DIR}/SDK.hpp")
    message(FATAL_ERROR
        "[CustomCraft] SDK introuvable dans ${SN2_SDK_DIR}.\n"
        "Lance : git submodule add https://github.com/Evasion3356/Subnautica-2-Cheat vendor/SN2SDK")
endif()

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/CraftHook.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src/
    "${SN2_SDK_DIR}"          # SDK.hpp (include-all header)
    "${SN2_SDK_DIR}/SDK"      # fichiers _classes.hpp, _structs.hpp individuels
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    mod_common_options
    "${UE4SS_DIR}/lib/UE4SS.lib"
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
    SUFFIX ".dll"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out/${PROJECT_NAME}"
)
