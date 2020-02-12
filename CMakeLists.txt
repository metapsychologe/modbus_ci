cmake_minimum_required(VERSION 3.10)

project(modbus_cli LANGUAGES C)
set(CMAKE_C_STANDARD 11)

add_executable(modbus_read_coils
  "main.c"
  "read.c")
target_link_libraries(modbus_read_coils modbus)
target_compile_definitions(modbus_read_coils PUBLIC READ_COILS)

add_executable(modbus_read_input_bits
  "main.c"
  "read.c")
target_link_libraries(modbus_read_input_bits modbus)
target_compile_definitions(modbus_read_input_bits PUBLIC READ_INPUT_BITS)

add_executable(modbus_read_registers
  "main.c"
  "read.c")
target_link_libraries(modbus_read_registers modbus)
target_compile_definitions(modbus_read_registers PUBLIC READ_HOLDING_REGISTERS)

add_executable(modbus_read_input_registers
  "main.c"
  "read.c")
target_link_libraries(modbus_read_input_registers modbus)
target_compile_definitions(modbus_read_input_registers PUBLIC READ_INPUT_REGISTERS)

