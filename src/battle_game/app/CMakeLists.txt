set(lib_name battle_game_${COMPONENT_NAME}_lib)
list(APPEND LIBRARY_LIST ${lib_name})

add_library(${lib_name})
file(GLOB source_files *.cpp *.h)
target_sources(${lib_name} PUBLIC ${source_files})
target_link_libraries(${lib_name} PUBLIC grassland battle_game_core_lib)
target_include_directories(${lib_name} PUBLIC ${BATTLE_GAME_EXTERNAL_INCLUDE_DIRS} ${BATTLE_GAME_INCLUDE_DIR})

set(LIBRARY_LIST ${LIBRARY_LIST} PARENT_SCOPE)

target_compile_definitions(${lib_name} PUBLIC BATTLE_GAME_ASSETS_DIR="${BATTLE_GAME_ASSETS_DIR}/")

PACK_SHADER_CODE(${lib_name})
