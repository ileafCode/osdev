
# MeduzaOS (UEFI)
## Current features
- [x] Basic ACPI support (finding tables. that's it.)
- [x] PCI (only AHCI driver)
- [x] FAT filesystem (FatFs)
- [x] Better syscalls
- [x] ELF support (no relocatables) 

## Building
### Prerequisites
- QEMU (emulator)
- NASM (assembler)
- GCC and G++
- WSL2 (for Windows users)

### Build Instructions (x86_64)
1. Run:
```bash
./run.sh
```