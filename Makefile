CROSS_COMPILE_PATH = $(HOME)/opt/cross/bin/
CC = $(CROSS_COMPILE_PATH)x86_64-elf-gcc
LD = $(CROSS_COMPILE_PATH)x86_64-elf-gcc
AS = $(CROSS_COMPILE_PATH)x86_64-elf-as
GRUB_FILE = grub-file
GRUB_MKRESCUE = grub-mkrescue
QEMU = qemu-system-x86_64


INCLUDES = -Iinclude -Iinclude/common -Iarch/i386 -Idrivers -Ikernel -Ikernel/memory -Ikernel/shell -Ikernel/system

SRCS_C   := $(shell find . -name '*.c')
SRCS_ASM := $(shell find . -name '*.s')
OBJS     := $(SRCS_C:.c=.o) $(SRCS_ASM:.s=.o)

CFLAGS  = -std=gnu99 -ffreestanding -O2 -Wall -Wextra $(INCLUDES) 
ASFLAGS = 
LDFLAGS = -T arch/i386/boot/linker.ld -ffreestanding -O2 -nostdlib -lgcc

TARGET_BIN = myos.bin
TARGET_ISO = myos.iso
ISODIR = isodir

.PHONY: all run clean multiboot_check

all: $(TARGET_ISO)

$(TARGET_ISO): $(TARGET_BIN) grub.cfg
	mkdir -p $(ISODIR)/boot/grub
	cp $(TARGET_BIN) $(ISODIR)/boot/$(TARGET_BIN)
	cp grub.cfg $(ISODIR)/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $@ $(ISODIR)

$(TARGET_BIN): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
	$(MAKE) multiboot_check

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

multiboot_check:
	@$(GRUB_FILE) --is-x86-multiboot2 $(TARGET_BIN) || (echo "Not Multiboot2 compliant!" && exit 1)


run: $(TARGET_ISO)
	$(QEMU) -cdrom $(TARGET_ISO) \
	         -m 1024M \
	         -cpu qemu64,+lm \
	         -serial stdio \
			 -d int,cpu_reset -D qemu.log -no-reboot -no-shutdown

clean:
	rm -f $(OBJS) $(TARGET_BIN) $(TARGET_ISO)
	rm -rf $(ISODIR)