#CFLAGS = -fno-stack-protector -std=c99 -march=i686 -ffreestanding -Wall -g
CFLAGS = -fno-stack-protector -ffreestanding -fno-builtin -g

boot1.img: c_init.bin
	dd if=$< of=$@ bs=512 count=1
	dd if=/dev/zero of=$@ skip=1 seek=1 bs=512 count=2879

c_init.bin: c_init.elf
	objcopy -R .pdr -R .comment -R.note -S -O binary $< $@

c_init.elf: c_init.o b.o bss.lds
	ld -nostdlib -g -o $@ -Tbss.lds c_init.o b.o 
c_init.o: c_init.s
	as -o $@ $<
b.o: b.c
	gcc $(CFLAGS) -c $<
b.s: b.c
	gcc -S $(CFLAGS) -c $<

c_init_by_boot_loader.bin: c_init_by_boot_loader.elf
	objcopy -R .pdr -R .comment -R.note -S -O binary $< $@

c_init_by_boot_loader.elf: c_init_by_boot_loader.o b_by_bootloader.o switch_vga_mode.o bss_dos.lds
	ld -nostdlib -g -o $@ -Tbss_dos.lds c_init_by_boot_loader.o b_by_bootloader.o switch_vga_mode.o
c_init_by_boot_loader.o: c_init_by_boot_loader.s
	as -o $@ $<
b_by_bootloader.o: b_by_bootloader.c
	gcc $(CFLAGS) -c $<
b_by_bootloader.s: b_by_bootloader.c
	gcc -S $(CFLAGS) $<
switch_vga_mode.o: switch_vga_mode.c
	gcc $(CFLAGS) -c $<
switch_vga_mode.s: switch_vga_mode.c
	gcc -S $(CFLAGS) $<

kloader.bin: kernel_loader.elf
	objcopy -R .pdr -R .comment -R.note -S -O binary $< $@

kernel_loader.elf: c_init_by_boot_loader.o kernel_loader.o 
	ld -nostdlib -g -o $@ -Tbss_dos.lds $^
#kernel_loader.o: kernel_loader.c elf.h
#	gcc -std=c99 $(CFLAGS) -c $<
kernel_loader.o: kernel_loader.s
	gcc -std=c99 $(CFLAGS) -c $<
kernel_loader.s: kernel_loader.c elf.h
	gcc -std=c99 $(CFLAGS) -o $@ -S $<

kernel.bin: kernel.elf
	objcopy -R .pdr -R .comment -R.note -S -O binary $< $@
kernel.elf: kernel.o
	ld -nostdlib -g -o $@ -Tbss_dos.lds $^
kernel.o: kernel.s
	as -o $@ $<

# enter protected mode kernel loader
kloaderp.bin: kloaderp.bin.elf
	objcopy -R .pdr -R .comment -R.note -S -O binary $< $@

kloaderp.bin.elf: kloader_init.o kernel_loader.o protected_code.o
	ld -nostdlib -g -o $@ -Tbss_dos.lds $^

kloader_init.o: kloader_init.S
	gcc $(CFLAGS) -o $@ -c $<

protected_code.o: protected_code.c
	gcc $(CFLAGS) -c $<
protected_code.s: protected_code.c
	gcc $(CFLAGS) -o $@ -S $<

p_kernel.elf: p_kernel.o
	ld -nostdlib -g -o $@ -Tk.ld $^

p_kernel.o: p_kernel.s
	as -o $@ $<

.PHONE: clean distclean

clean:
	rm -rf *.o *.elf *.bin 
distclean:
	rm -rf *.img
