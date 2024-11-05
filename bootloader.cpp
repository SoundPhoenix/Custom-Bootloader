// FILE: bootloader/bootloader.cpp
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

EFI_STATUS LoadFile(EFI_HANDLE image, EFI_SYSTEM_TABLE* st, CHAR16* path, EFI_PHYSICAL_ADDRESS* address, UINTN* size) {
    EFI_STATUS status;
    EFI_FILE_IO_INTERFACE* io;
    EFI_FILE* root;
    EFI_FILE* file;
    EFI_FILE_INFO* info;
    UINTN buffer_size;
    void* buffer;

    status = st->BootServices->HandleProtocol(image, &gEfiSimpleFileSystemProtocolGuid, (void**)&io);
    if (EFI_ERROR(status)) return status;

    status = io->OpenVolume(io, &root);
    if (EFI_ERROR(status)) return status;

    status = root->Open(root, &file, path, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) return status;

    buffer_size = sizeof(EFI_FILE_INFO) + 1024;
    info = (EFI_FILE_INFO*)AllocatePool(buffer_size);
    status = file->GetInfo(file, &gEfiFileInfoGuid, &buffer_size, info);
    if (EFI_ERROR(status)) return status;

    buffer_size = info->FileSize;
    buffer = AllocatePool(buffer_size);
    status = file->Read(file, &buffer_size, buffer);
    if (EFI_ERROR(status)) return status;

    *address = (EFI_PHYSICAL_ADDRESS)buffer;
    *size = buffer_size;

    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE* st) {
    InitializeLib(image, st);

    BootParams params;
    EFI_STATUS status;

    status = LoadFile(image, st, KERNEL_PATH, &params.kernel_start, &params.initrd_size);
    if (EFI_ERROR(status)) {
        Print(L"Failed to load kernel: %r\n", status);
        return status;
    }

    status = LoadFile(image, st, INITRD_PATH, &params.initrd_start, &params.initrd_size);
    if (EFI_ERROR(status)) {
        Print(L"Failed to load initrd: %r\n", status);
        return status;
    }

    params.cmdline = CMDLINE;

    // Here you would normally jump to the kernel entry point
    // For simplicity, we just print a success message
    Print(L"Kernel and initrd loaded successfully.\n");

    return EFI_SUCCESS;
}