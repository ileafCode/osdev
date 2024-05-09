
std_libc="libc/sys/crt.o libc/sys/mouse.o libc/sys/fb.o libc/sys/wm.o libc/string.o libc/stdlib.o libc/math.o libc/stdio.o"

g++ -O0 -c libc/sys/crt.c -o libc/sys/crt.o -ffreestanding -fpic -Wall -Wextra
g++ -O0 -c libc/sys/mouse.c -o libc/sys/mouse.o -ffreestanding -fpic -Wall -Wextra
g++ -O0 -c libc/sys/fb.c -o libc/sys/fb.o -ffreestanding -fpic -Wall -Wextra
g++ -O0 -c libc/sys/wm.c -o libc/sys/wm.o -ffreestanding -fpic -Wall -Wextra
g++ -O0 -c libc/string.c -o libc/string.o -ffreestanding -fpic -Wall -Wextra
g++ -O0 -c libc/stdlib.c -o libc/stdlib.o -ffreestanding -fpic -Wall -Wextra
g++ -O0 -c libc/math.c -o libc/math.o -ffreestanding -fpic -Wall -Wextra
g++ -O0 -c libc/stdio.c -o libc/stdio.o -ffreestanding -fpic -Wall -Wextra

g++ -O0 -c -I libc -nostdlib -fno-exceptions -fno-stack-protector -mno-red-zone -pipe raycaster.c -o bin/TEST00.A
ld -m elf_x86_64 -Ttext 0x1000000 $std_libc bin/TEST00.A -o bin/TEST00.O

g++ -O0 -c -I libc -nostdlib -fno-exceptions -fno-stack-protector -mno-red-zone -pipe wintst.c -o bin/wintst.a
ld -m elf_x86_64 -Ttext 0x1000000 $std_libc bin/wintst.a -o bin/wintst.o

nasm -felf64 asmt.asm -o bin/asmtst.a
ld -m elf_x86_64 -Ttext 0x1000000 bin/asmtst.a -o bin/asmtst.o