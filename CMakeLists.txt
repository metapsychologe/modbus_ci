cmake_minimum_required(VERSION 3.10)

project(modbus_cli LANGUAGES C)
set(CMAKE_C_STANDARD 11)

add_executable(modbus_read_coils
  "read.c"
  "modbus_cli.c")
target_link_libraries(modbus_read_coils modbus)
target_compile_definitions(modbus_read_coils PUBLIC READ_COILS)

add_executable(modbus_read_discrete_inputs
  "read.c"
  "modbus_cli.c")
target_link_libraries(modbus_read_discrete_inputs modbus)
target_compile_definitions(modbus_read_discrete_inputs PUBLIC READ_INPUT_BITS)

add_executable(modbus_read_registers
  "read.c"
  "modbus_cli.c")
target_link_libraries(modbus_read_registers modbus)
target_compile_definitions(modbus_read_registers PUBLIC READ_HOLDING_REGISTERS)

add_executable(modbus_read_input_registers
  "read.c"
  "modbus_cli.c")
target_link_libraries(modbus_read_input_registers modbus)
target_compile_definitions(modbus_read_input_registers PUBLIC READ_INPUT_REGISTERS)

