LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := modmenu

# --- 1. DAFTARKAN SEMUA FOLDER HEADER DI SINI ---
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Includes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Includes/Dobby
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ImGui

# --- 2. DAFTARKAN SEMUA FILE .cpp KAMU ---
LOCAL_SRC_FILES := main.cpp \
                   ImGui/imgui.cpp \
                   ImGui/imgui_draw.cpp \
                   ImGui/imgui_widgets.cpp \
                   ImGui/imgui_tables.cpp \
                   ImGui/imgui_demo.cpp \
                   ImGui/imgui_impl_android.cpp \
                   ImGui/imgui_impl_opengl3.cpp

# --- 3. TAMBAHKAN LIBRARY DOBBY ---
# Pastikan file libdobby.a ada di folder Includes/Dobby/
LOCAL_LDFLAGS += $(LOCAL_PATH)/Includes/Dobby/libdobby.a

LOCAL_LDLIBS := -llog -landroid -lGLESv3 -lEGL -ldl

LOCAL_CFLAGS := -Wno-error=format-security -fvisibility=hidden -fvisibility-inlines-hidden -fstack-protector-all
LOCAL_CPPFLAGS := -std=c++17 -frtti -fexceptions

include $(BUILD_SHARED_LIBRARY)
