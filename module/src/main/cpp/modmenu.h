#ifndef MODMENU_H
#define MODMENU_H

#include <jni.h>
#include "Includes/Variables.h"
#include "Includes/obfuscate.h"

#define targetLibName OBFUSCATE("libil2cpp.so")

// Nisaa ganti jadi bool biar gak berantem sama main.cpp sayang 🩷
extern bool enable_hack; 

extern "C" {
    int isGame(JNIEnv *env, jstring appDataDir);
    void drawMenu();
    void initModMenu(void *drawMenu);
}

#endif
