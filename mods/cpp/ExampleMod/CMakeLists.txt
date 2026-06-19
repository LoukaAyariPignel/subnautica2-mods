project(ExampleMod VERSION 1.0.0)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    mod_common_options
    "${UE4SS_DIR}/lib/UE4SS.lib"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
    SUFFIX ".dll"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out/${PROJECT_NAME}"
)
