#ifndef ZYCHEATS_SGUYS_FUNCTIONS_H
#define ZYCHEATS_SGUYS_FUNCTIONS_H

void (*PurchaseRealMoney) (void* instance, monoString* itemId, monoString* receipt, void* callback);

void Pointers() {
    PurchaseRealMoney = (void(*)(void*, monoString*, monoString*, void*)) (g_il2cppBaseMap.startAddress + string2Offset(OBFUSCATE("0xE7AADC")));
}

void Patches() {
    PATCH_SWITCH("0x10A69A0", "200080D2C0035FD6", showAllItems);
    PATCH_SWITCH("0xF148A4", "E07C80D2C0035FD6", freeItems);
}

#endif //ZYCHEATS_SGUYS_FUNCTIONS_H
