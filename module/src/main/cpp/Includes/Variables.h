#ifndef VARIABLES_H
#define VARIABLES_H

#include "../ImGui/imgui.h"

// Struktur data untuk menyimpan status semua fitur hack kamu
struct MyOptions {
    // Tombol ON/OFF
    bool esp_box = false;
    bool esp_line = false;
    bool esp_circle = false;
    bool esp_text = false;
    bool god_mode = false;
    bool unlimited_jump = false;
    
    
    // Pengatur Warna (Default Merah: R=1.0, G=0.0, B=0.0, A=1.0)
    ImVec4 esp_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    
    // Slider (Contoh jika ingin mengatur jarak ESP)
    float esp_distance = 100.0f;
};

// 'extern' artinya variabel ini bisa dipakai bareng-bareng oleh file lain
extern MyOptions options;

#endif // VARIABLES_H
