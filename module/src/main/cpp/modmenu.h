#ifndef MODMENU_H
#define MODMENU_H

#include <jni.h>
#include "Includes/Variables.h"
#include "Includes/obfuscate.h"

#define targetLibName OBFUSCATE("libil2cpp.so")

// Nisaa sudah samakan tipenya jadi bool supaya tidak berantem sama main.cpp
static bool enable_hack = false; 

// --- DEKLARASI FUNGSI ---
int isGame(JNIEnv *env, jstring appDataDir);
void drawMenu();
void initModMenu(void *drawMenu);

#endif // MODMENU_H
