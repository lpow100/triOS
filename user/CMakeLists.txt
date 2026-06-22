cmake_minimum_required(VERSION 3.16)
project(UserSpace C)

# Pull in our sub-applications
add_subdirectory(shell)

# Custom target to collect all the baked .sef files into a USTAR archive
add_custom_target(initrd_ustar ALL
    COMMAND tar --format=ustar -cvf ${CMAKE_BINARY_DIR}/user/initrd.ustar -C ${CMAKE_BINARY_DIR}/user/shell shell.sef
    COMMENT "Packing SEF binaries into initrd.ustar"
    DEPENDS shell.bin
)