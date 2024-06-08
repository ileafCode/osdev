
SEARCH_DIR="libc"
OUTPUT_DIR="bin/libc"
LIBRARY_NAME="bin/meduzalibc.a"

mkdir -p "$OUTPUT_DIR"

echo "Compiling all .c files in $SEARCH_DIR and its subdirectories:"
find "$SEARCH_DIR" -type f -name "*.c" | while read -r file; do
  filename=$(basename "$file" .c)
  gcc -I libc/include -c "$file" -o "$OUTPUT_DIR/$filename.o" -O2 -ffreestanding -fpic -Wall -Wextra
  echo "Compiled $file to $OUTPUT_DIR/$filename.o"
done

OBJ_FILES=$(find "$OUTPUT_DIR" -type f -name "*.o")

gcc -ffreestanding -fpic -Wall -Wextra -O0 -I libc/include -c init.c -o obj/init.a 
ld -m elf_x86_64 -Ttext 0x1000000 $OBJ_FILES obj/init.a -o bin/init