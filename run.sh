clear

cd user_prog
./build.sh
cd ../

gcc fsman.c -o fsman -lm
cd gnu-efi
make
make bootloader
cd ../kernel
rm -r lib
make kernel
make buildimg
cd ../
./fsman user_prog/TEXT.TGA user_prog/mous.tga user_prog/bin/wintst.o user_prog/bin/asmtst.o
cd kernel
make run
cd ../
