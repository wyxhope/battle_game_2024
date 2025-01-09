file(GLOB HEADERS "*.h")

set(UNITS_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/units.h)

message(STATUS ${UNITS_HEADER_FILE})

file(WRITE ${UNITS_HEADER_FILE} "// This is a generated file\n#pragma once\n")

foreach (header ${HEADERS})
    get_filename_component(header_name ${header} NAME)
    file(APPEND ${UNITS_HEADER_FILE} "#include \"battle_game/core/units/${header_name}\"\n")
endforeach ()

target_include_directories(${lib_name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
