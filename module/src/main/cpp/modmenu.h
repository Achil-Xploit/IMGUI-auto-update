#ifndef MODMENU_H
#define MODMENU_H

#include <jni.h>
#include "Includes/Variables.h"
#include "Includes/obfuscate.h"

#define targetLibName OBFUSCATE("libil2cpp.so")

// Nisaa samakan tipenya jadi bool
extern bool enable_hack; 

// --- DEKLARASI SAJA (Biar tidak Redefinition) ---
extern "C" {
    int isGame(JNIEnv *env, jstring appDataDir);
    void drawMenu();
    void initModMenu(void *drawMenu);
}

#endif // MODMENU_H
