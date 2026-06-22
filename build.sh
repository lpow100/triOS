#!/bin/bash
cd build

cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..

cmake --build .

qemu-system-x86_64 -cdrom myos.iso \
	         -m 1024M \
	         -cpu qemu64,+lm \
	         -serial stdio \
			 -d int,cpu_reset -D qemu.log -no-reboot -no-shutdown

cd ..