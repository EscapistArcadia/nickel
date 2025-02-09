#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS status = EFI_SUCCESS;

    InitializeLib(ImageHandle, SystemTable);

    status = uefi_call_wrapper(SystemTable->BootServices->SetWatchdogTimer, 4, 0, 0, 0, NULL);
    if (status != EFI_SUCCESS) {
        Print(L"[%d] Bad status: %r", __LINE__, status);
        return status;
    }

    status = uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);
    if (status != EFI_SUCCESS) {
        Print(L"[%d] Bad status: %r", __LINE__, status);
        return status;
    }
    
    Print(L"[%d] Good status: %r", __LINE__, status);

    while (1);

    return status;
}