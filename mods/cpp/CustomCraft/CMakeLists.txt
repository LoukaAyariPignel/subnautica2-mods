project(CustomCraft VERSION 1.0.0)

# Dumper-7 SDK — généré depuis Subnautica2 avec Dumper-7.
# Placer le SDK dans vendor/SN2SDK/ (voir README.md#prérequis).
set(SN2_SDK_DIR "${CMAKE_SOURCE_DIR}/vendor/SN2SDK" CACHE PATH "Dumper-7 SDK path")

if(NOT EXISTS "${SN2_SDK_DIR}/SDK/Subnautica2_classes.hpp")
    message(FATAL_ERROR
        "[CustomCraft] SDK Dumper-7 introuvable dans ${SN2_SDK_DIR}.\n"
        "Lance Dumper-7 sur Subnautica2 pour générer le SDK.")
endif()

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/CraftHook.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src/
    "${SN2_SDK_DIR}"
    "${SN2_SDK_DIR}/SDK"
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
