#include <cstring>
#include <jni.h>
#include <pthread.h>
#include <cstdio>
#include <unistd.h>
#include <dlfcn.h>
#include <list>
#include <vector>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <dobby.h> 

#include "modmenu.h"
#include "zygisk.hpp"
#include "Includes/Variables.h"
#include "Includes/ESP.h" // Pastikan include ini ada

using zygisk::Api;
using zygisk::AppSpecializeArgs;

bool enable_hack = false;


// Struktur data posisi
struct Vector3 { float x, y, z; };
struct Vector2 { float x, y; };

// --- 1. IL2CPP API SUPPORT NAMESPACE ---
typedef void* (*il2cpp_domain_get)();
typedef void** (*il2cpp_domain_get_assemblies)(void* domain, size_t* size);
typedef void* (*il2cpp_assembly_get_image)(void* assembly);
typedef void* (*il2cpp_class_from_name)(void* image, const char* namespaze, const char* name);
typedef void* (*il2cpp_class_get_method_from_name)(void* klass, const char* name, int argsCount);

void* get_method_auto(const char* namespaze, const char* className, const char* methodName, int argsCount) {
    void* handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (!handle) return nullptr;

    auto f_domain_get = (il2cpp_domain_get)dlsym(handle, "il2cpp_domain_get");
    auto f_domain_get_assemblies = (il2cpp_domain_get_assemblies)dlsym(handle, "il2cpp_domain_get_assemblies");
    auto f_assembly_get_image = (il2cpp_assembly_get_image)dlsym(handle, "il2cpp_assembly_get_image");
    auto f_class_from_name = (il2cpp_class_from_name)dlsym(handle, "il2cpp_class_from_name");
    auto f_class_get_method_from_name = (il2cpp_class_get_method_from_name)dlsym(handle, "il2cpp_class_get_method_from_name");

    size_t size;
    void** assemblies = f_domain_get_assemblies(f_domain_get(), &size);
    for (size_t i = 0; i < size; ++i) {
        void* image = f_assembly_get_image(assemblies[i]);
        void* klass = f_class_from_name(image, namespaze, className); 
        if (klass) return f_class_get_method_from_name(klass, methodName, argsCount);
    }
    return nullptr;
}

// --- 2. HOOK HANDLER (Unlimited Jump & ESP) ---
uint8_t (*old_Jump)(void* instance);
uint8_t hook_Jump(void* instance) {
    if (instance != nullptr) {
        // --- LOGIKA ESP ---
        if (options.esp_box) {
            // Ambil koordinat dunia (Contoh offset 0x30, sesuaikan dump.cs kamu)
            Vector3 worldPos = *(Vector3*)((uintptr_t)instance + 0x30); 
            
            // Gambar Box mengikuti koordinat karakter
            // Catatan: Ini masih koordinat dunia, belum WorldToScreen
            ESP::DrawBox(worldPos.x, worldPos.y, 100.0f, 200.0f, options.esp_color);
        }

        // --- LOGIKA JUMP ---
        if (options.unlimited_jump) { 
            return 0; 
        }
    }
    return old_Jump(instance);
}

// --- 3. HACK THREAD ---
void *hack_thread(void *) {
    initModMenu((void *)drawMenu);

    while (dlopen("libil2cpp.so", RTLD_LAZY) == nullptr) {
        sleep(1);
    }
    sleep(5); 

    void* target = get_method_auto("KradGame.Logic", "CharacterLogic", "get_AirJumpCount", 0);
    
    if (target) {
        DobbyHook(target, (void *)hook_Jump, (void **)&old_Jump);
        LOGI("Hook CharacterLogic Berhasil!");
    } else {
        LOGE("Gagal menemukan Method get_AirJumpCount!");
    }
    return nullptr;
}

// --- 4. ZYGISK MODULE ---
class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override { env_ = env; }
    void preAppSpecialize(AppSpecializeArgs *args) override {
        if (!args || !args->nice_name) return;
        enable_hack = isGame(env_, args->app_data_dir);
    }
    void postAppSpecialize(const AppSpecializeArgs *) override {
        if (enable_hack) {
            pthread_t ntid;
            pthread_create(&ntid, nullptr, hack_thread, nullptr);
        }
    }
private:
    JNIEnv *env_{};
};

REGISTER_ZYGISK_MODULE(MyModule)
