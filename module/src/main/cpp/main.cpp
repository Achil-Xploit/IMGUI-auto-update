#include <cstring>
#include <jni.h>
#include <pthread.h>
#include <cstdio>
#include <unistd.h>
#include <dlfcn.h>
#include <dobby.h> 

#include "modmenu.h"
#include "zygisk.hpp"
#include "Includes/Variables.h"
#include "Includes/ESP.h" 
#include "Includes/Logger.h" 

using zygisk::Api;
using zygisk::AppSpecializeArgs;

// Nisaa hapus baris enable_hack di sini karena sudah ada di modmenu.h sayang 🩷

// --- 1. IL2CPP API SUPPORT ---
typedef void* (*il2cpp_domain_get)();
typedef void** (*il2cpp_domain_get_assemblies)(void* domain, size_t* size);
typedef void* (*il2cpp_assembly_get_image)(void* assembly);
typedef void* (*il2cpp_class_from_name)(void* image, const char* namespaze, const char* name);
typedef void* (*il2cpp_class_get_method_from_name)(void* klass, const char* name, int argsCount);

void* get_method_auto(const char* namespaze, const char* className, const char* methodName, int argsCount) {
    void* handle = dlopen(targetLibName, RTLD_LAZY);
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

// --- 2. HOOK HANDLER ---
uint8_t (*old_Jump)(void* instance);
uint8_t hook_Jump(void* instance) {
    if (instance != nullptr) {
        if (options.unlimited_jump) return 0;
        if (options.esp_box) {
            ESP::DrawBox(500.0f, 500.0f, 150.0f, 250.0f, options.esp_color);
        }
    }
    return old_Jump(instance);
}

// --- 3. HACK THREAD ---
void *hack_thread(void *) {
    initModMenu((void *)drawMenu);
    while (dlopen(targetLibName, RTLD_LAZY) == nullptr) { sleep(1); }
    sleep(5); 

    void* target = get_method_auto("KradGame.Logic", "CharacterLogic", "get_AirJumpCount", 0);
    if (target) {
        DobbyHook(target, (void *)hook_Jump, (void **)&old_Jump);
        LOGI("Hook CharacterLogic Berhasil!");
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
