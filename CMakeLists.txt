cmake_minimum_required(VERSION 3.10)
project(NutCpu)

set(PYTHON_SCRIPT "${CMAKE_SOURCE_DIR}/tabella.py")

set(GENERATED_C_FILE "${CMAKE_BINARY_DIR}/pitoDump.c")

add_custom_command(
    OUTPUT ${GENERATED_C_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Running Python script to generate C code..."
    COMMAND ${CMAKE_COMMAND} -E env python3 ${PYTHON_SCRIPT} ${GENERATED_C_FILE}
    DEPENDS ${PYTHON_SCRIPT}
    COMMENT "Generating C code from Python script"
    VERBATIM
)
add_custom_target(
    GenerateCode ALL
    DEPENDS ${GENERATED_C_FILE}
)

add_executable(NutCpu main.c operations.c ${GENERATED_C_FILE})

add_dependencies(NutCpu GenerateCode)

target_include_directories(NutCpu PRIVATE "${CMAKE_SOURCE_DIR}")