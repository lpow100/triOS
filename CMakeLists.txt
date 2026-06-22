cmake_minimum_required(VERSION 3.16)
project(TriOS C ASM)

set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
add_compile_options(-fno-pie -fno-PIE)

# 1. Separate your source files by type
file(GLOB_RECURSE C_SOURCES 
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
    kernel/*.c drivers/*.c arch/*.c include/*.c
)

file(GLOB_RECURSE ASM_SOURCES 
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
    kernel/*.s drivers/*.s arch/*.s
)

# 2. Compile your assembly files inside an isolated Object Library
add_library(kernel_asm OBJECT ${ASM_SOURCES})

# 3. FORCE CMake to compile these specific assembly files using the GCC driver!
set_source_files_properties(${ASM_SOURCES} PROPERTIES LANGUAGE C)

# 4. Define the main OS binary executable
add_executable(myos.bin ${C_SOURCES} $<TARGET_OBJECTS:kernel_asm>)

# 5. Add header search paths
target_include_directories(myos.bin PRIVATE 
    include include/common arch/i386 drivers kernel kernel/memory kernel/shell kernel/system
)

# 6. Set compilation and freestanding strict bare-metal flags
target_compile_options(myos.bin PRIVATE -ffreestanding -O2 -mno-sse -fno-pie -fno-PIE)
target_link_options(myos.bin PRIVATE 
    -T ${CMAKE_CURRENT_SOURCE_DIR}/arch/i386/boot/linker.ld 
    -ffreestanding -nostdlib -static -no-pie
)

# 7. Step into the user directory (this will build shell.bin and shell.sef)
add_subdirectory(user)

# 2. Update your bootable ISO generation step to point to that exact file
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/myos.iso
    COMMAND mkdir -p isodir/boot/grub
    COMMAND cp ${CMAKE_BINARY_DIR}/myos.bin isodir/boot/myos.bin
    # Copy the absolute tar file we just built into the ISO directory
    COMMAND cp ${CMAKE_BINARY_DIR}/user/initrd.ustar isodir/boot/initrd.ustar
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/grub.cfg isodir/boot/grub/grub.cfg
    COMMAND grub-mkrescue -o ${CMAKE_BINARY_DIR}/myos.iso isodir
    DEPENDS myos.bin shell.bin initrd_ustar
    COMMENT "Packaging latest binaries into myos.iso via grub-mkrescue"
)

add_custom_target(image ALL DEPENDS ${CMAKE_BINARY_DIR}/myos.iso)