cmake_minimum_required(VERSION 3.16)
project(user_shell C ASM)

file(GLOB SOURCES "*.c" "*.S")
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE C)

add_executable(shell.bin ${SOURCES})
target_include_directories(shell.bin PRIVATE ../common)
target_compile_options(shell.bin PRIVATE -ffreestanding -O2)

target_link_options(shell.bin PRIVATE 
    -ffreestanding 
    -nostdlib 
    -Wl,-Ttext=0x400000,--oformat=binary,-e,_start
)

add_custom_command(TARGET shell.bin POST_BUILD
    # Force the absolute path to the tool
    COMMAND ${CMAKE_BINARY_DIR}/../tools/bin2sef shell.bin shell.sef
    # Force the shell to execute this command from the directory where shell.bin lives
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Baking shell.bin into native Star Executable Format (shell.sef)"
)