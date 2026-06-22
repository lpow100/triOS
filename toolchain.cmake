# Identify the target operating system environment properties
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Force the exact absolute paths to your cross-compiler tools
set(CMAKE_C_COMPILER "$ENV{HOME}/opt/cross/bin/x86_64-elf-gcc")
set(CMAKE_ASM_COMPILER "$ENV{HOME}/opt/cross/bin/x86_64-elf-as")

# Force CMake to skip trying to compile a test program (which always fails without a standard C library)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)