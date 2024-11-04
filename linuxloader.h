#ifndef LINUXLOADER_H
#define LINUXLOADER_H

#include <efi.h>
#include <efilib.h>

#define KERNEL_PATH L"\\vmlinuz"
#define INITRD_PATH L"\\initrd.img"
#define CMDLINE     "root=/dev/sda1 quiet splash"

struct BootParams {
    EFI_PHYSICAL_ADDRESS kernel_start;
    EFI_PHYSICAL_ADDRESS initrd_start;
    UINTN initrd_size;
    const char* cmdline;
};

EFI_STATUS LoadFile(EFI_HANDLE image, EFI_SYSTEM_TABLE* st, CHAR16* path, EFI_PHYSICAL_ADDRESS* address, UINTN* size);
EFI_STATUS StartLinux(EFI_SYSTEM_TABLE* st, BootParams* params);

#endif
