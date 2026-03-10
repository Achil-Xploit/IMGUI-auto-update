LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := modmenu

# --- 1. DAFTARKAN SEMUA FOLDER HEADER DI SINI ---
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Includes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Includes/Dobby
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ImGui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ImGui/backends

# --- 2. DAFTARKAN SEMUA FILE .cpp KAMU ---
LOCAL_SRC_FILES := main.cpp \
                   KittyMemory/KittyMemory.cpp \
                   KittyMemory/MemoryPatch.cpp \
                   KittyMemory/MemoryBackup.cpp \
                   KittyMemory/KittyUtils.cpp \
                   ImGui/imgui.cpp \
                   ImGui/imgui_demo.cpp \
                   ImGui/imgui_draw.cpp \
                   ImGui/imgui_tables.cpp \
                   ImGui/imgui_widgets.cpp \
                   ImGui/backends/imgui_impl_opengl3.cpp \
                   ImGui/backends/imgui_impl_android.cpp

# --- 3. TAMBAHKAN LIBRARY DOBBY ---
LOCAL_LDFLAGS += $(LOCAL_PATH)/Includes/Dobby/libdobby.a

LOCAL_LDLIBS := -llog -landroid -lGLESv3 -lEGL -ldl

LOCAL_CFLAGS := -Wno-error=format-security -Wno-format-security -fvisibility=hidden -fvisibility-inlines-hidden -fstack-protector-all

LOCAL_CPPFLAGS := -std=c++17 -frtti -fexceptions

include $(BUILD_SHARED_LIBRARY)
