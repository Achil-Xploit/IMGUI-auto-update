#ifndef MODMENU_H
#define MODMENU_H

#include <jni.h>
#include "Includes/Variables.h"
#include "Includes/obfuscate.h"

#define targetLibName OBFUSCATE("libil2cpp.so")

// --- VARIABEL GLOBAL ---
// Pakai bool supaya pas sama main.cpp sayang
static bool enable_hack = false; 

// --- DEKLARASI FUNGSI ---
int isGame(JNIEnv *env, jstring appDataDir);
void drawMenu();
void initModMenu(void *drawMenu);

#endif // MODMENU_H
