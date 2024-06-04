
std_libc="libc/sys/crt.o libc/sys/syscall.o libc/sys/kbd.o libc/stdlib.o libc/math.o"

g++ -O0 -c libc/sys/crt.c -o libc/sys/crt.o -ffreestanding -fpic -Wall -Wextra
g++ -O0 -c libc/sys/syscall.c -o libc/sys/syscall.o -ffreestanding -fpic -Wall -Wextra
g++ -O0 -c libc/sys/kbd.c -o libc/sys/kbd.o -ffreestanding -fpic -Wall -Wextra
g++ -O0 -c libc/stdlib.c -o libc/stdlib.o -ffreestanding -fpic -Wall -Wextra
g++ -O0 -c libc/math.c -o libc/math.o -ffreestanding -fpic -Wall -Wextra

g++ -O0 -I libc -c init.c -o bin/a/init.a -ffreestanding -fpic -Wall -Wextra
ld -m elf_x86_64 -Ttext 0x1000000 $std_libc bin/a/init.a -o bin/init