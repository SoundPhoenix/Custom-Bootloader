#include <efi.h>
#include <efilib.h>
#include "linuxloader.h"

EFI_STATUS LoadFile(EFI_HANDLE image, EFI_SYSTEM_TABLE* st, CHAR16* path, EFI_PHYSICAL_ADDRESS* address, UINTN* size) {
    EFI_FILE_IO_INTERFACE* io;
    EFI_FILE_HANDLE root;
    EFI_FILE_HANDLE file;
    EFI_LOADED_IMAGE* loadedImage;
    
    st->BootServices->HandleProtocol(image, &gEfiLoadedImageProtocolGuid, (void**)&loadedImage);
    st->BootServices->HandleProtocol(loadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&io);
    
    io->OpenVolume(io, &root);
    if (root->Open(root, &file, path, EFI_FILE_MODE_READ, 0) != EFI_SUCCESS) {
        Print(L"File %s not found.\n", path);
        return EFI_NOT_FOUND;
    }

    EFI_FILE_INFO* info;
    UINTN infoSize = sizeof(EFI_FILE_INFO) + 1024;
    st->BootServices->AllocatePool(EfiLoaderData, infoSize, (void**)&info);
    file->GetInfo(file, &gEfiFileInfoGuid, &infoSize, info);

    *size = info->FileSize;
    st->BootServices->AllocatePages(AllocateAnyPages, EfiLoaderData, (*size + 0xFFF) / 0x1000, address);

    file->Read(file, size, (void*)*address);
    file->Close(file);
    
    return EFI_SUCCESS;
}

EFI_STATUS StartLinux(EFI_SYSTEM_TABLE* st, BootParams* params) {
    typedef void (*KernelEntry)(BootParams*);
    KernelEntry entry = (KernelEntry)(params->kernel_start);
    entry(params);
    return EFI_SUCCESS;
}

EFI_STATUS efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *st) {
    InitializeLib(image, st);

    BootParams params;
    EFI_PHYSICAL_ADDRESS kernel_start;
    UINTN kernel_size;
    
    if (LoadFile(image, st, KERNEL_PATH, &kernel_start, &kernel_size) != EFI_SUCCESS) {
        Print(L"Failed to load kernel\n");
        return EFI_LOAD_ERROR;
    }
    params.kernel_start = kernel_start;

    EFI_PHYSICAL_ADDRESS initrd_start;
    UINTN initrd_size;
    if (LoadFile(image, st, INITRD_PATH, &initrd_start, &initrd_size) != EFI_SUCCESS) {
        Print(L"Failed to load initrd\n");
        return EFI_LOAD_ERROR;
    }
    params.initrd_start = initrd_start;
    params.initrd_size = initrd_size;

    params.cmdline = CMDLINE;

    return StartLinux(st, &params);
}
