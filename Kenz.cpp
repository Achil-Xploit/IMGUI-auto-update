#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "KenzGUI/CallGUI_.h"
#include "DrawMenu.h"
#define targetLib "libil2cpp.so"
#include "Hooks.h"
#include "Zygisk/zygisk.h"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;
 



// ===================================================== //
#define targetPackageName OBFUSCATE("com.dois.greedgame") // Here add your game package name ..... For example purpose i will use sniper 3d game 

uintptr_t G_IL2CPP;


#include <ctime>
#include <cstdio>

void DrawNetBackground(const ImVec2& window_pos, const ImVec2& window_size, float time) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Parameters for the net
    const float grid_spacing = 40.0f; // Distance between grid points
    const float amplitude = 5.0f;     // Amount of vertical oscillation for animation
    const int cols = static_cast<int>(window_size.x / grid_spacing) + 2;
    const int rows = static_cast<int>(window_size.y / grid_spacing) + 2;

    ImU32 line_color = ImGui::GetColorU32(ImVec4(0.0f, 0.6f, 0.0f, 0.6f)); // Semi-transparent green
    ImU32 point_color = ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 0.8f)); // Bright green for points

    // Store points with animation offset
    std::vector<ImVec2> points(rows * cols);

    // Calculate points with vertical wave animation
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            float x = window_pos.x + col * grid_spacing;
            float y = window_pos.y + row * grid_spacing;

            // Animate y with sine wave based on time and position for glowing wave effect
            float offset = sinf(time * 3.0f + (col + row) * 0.5f) * amplitude;
            points[row * cols + col] = ImVec2(x, y + offset);
        }
    }

    // Draw horizontal lines
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols - 1; ++col) {
            draw_list->AddLine(points[row * cols + col], points[row * cols + col + 1], line_color, 1.0f);
        }
    }

    // Draw vertical lines
    for (int col = 0; col < cols; ++col) {
        for (int row = 0; row < rows - 1; ++row) {
            draw_list->AddLine(points[row * cols + col], points[(row + 1) * cols + col], line_color, 1.0f);
        }
    }

    // Draw points
    for (const auto& pt : points) {
        draw_list->AddCircleFilled(pt, 2.0f, point_color);
    }
}


ImVec4 RainbowColor(float t) {
    float r = sinf(t * 6.283185f) * 0.5f + 0.5f;
    float g = sinf(t * 6.283185f + 2.094f) * 0.5f + 0.5f;
    float b = sinf(t * 6.283185f + 4.188f) * 0.5f + 0.5f;
    return ImVec4(r, g, b, 1.0f);
}

time_t GetExpiryTimestamp(const char* expiry_date_str) {
    // expiry_date_str format is "DD-MM-YY"
    struct tm expiry_tm = {0};
    int day, month, year;
    if (sscanf(expiry_date_str, "%d-%d-%d", &day, &month, &year) != 3) {
        return 0; // invalid format fallback, never expires
    }
    expiry_tm.tm_mday = day;
    expiry_tm.tm_mon = month - 1; // tm_mon is 0-11
    expiry_tm.tm_year = (year < 100) ? (year + 100) : year; // 2000-based year (e.g., 25 -> 2025)
    expiry_tm.tm_hour = 0;
    expiry_tm.tm_min = 0;
    expiry_tm.tm_sec = 0;
    expiry_tm.tm_isdst = -1; // let system determine
    return mktime(&expiry_tm);
}


void DrawMenu() {
    // Set your expiry date here in DD-MM-YY format
    static time_t expiry_timestamp = GetExpiryTimestamp("28-10-25"); // Set your ModMenu Expiry date...so it will automatically expire your ModMenu according to date you set date/month/year
    
    time_t now = time(nullptr);
    ImVec2 window_size = ImGui::GetIO().DisplaySize;

    // If expired, show expiry message centered and skip rest of menu
    if (now > expiry_timestamp && expiry_timestamp != 0) {
        ImGui::SetNextWindowBgAlpha(0.75f);
        ImGui::SetNextWindowPos(ImVec2(window_size.x / 2, window_size.y / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::Begin("👿EXPIRED👿", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "👿 : 😈 ModMenu is expired 😈");
        ImGui::End();
        return; // Prevent drawing the rest of the menu when expired
    }
		
    const ImVec2 windowDim = ImVec2(700, 700);
    ImGui::SetNextWindowSize(windowDim, ImGuiCond_Once);

    if (ImGui::Begin(ICON_FA_GAMEPAD "✩ 𝐓𝐇𝐄 𝐆𝐇𝐎𝐒𝐓 ✩ [x64/x32]")) {
		ImVec2 pos = ImGui::GetWindowPos();   // <-- Declare pos here
        ImVec2 size = ImGui::GetWindowSize(); // <-- Declare size here
		float Meow = ImGui::GetTime();
		DrawNetBackground(pos, size, Meow);
        static int activeTab = 0;
        float windowWidth = ImGui::GetContentRegionAvail().x;

        // Tab buttons layout
        float spacing = 8.0f;
        int tabCount = 3;
        float buttonWidth = (windowWidth - spacing * (tabCount - 1)) / tabCount;
        float buttonHeight = 48.0f;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        float time = ImGui::GetTime();

        auto TabButton = [&](const char* label, int id) {
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImGuiStyle& style = ImGui::GetStyle();

            float oldRounding = style.FrameRounding;
            style.FrameRounding = 0.0f;

            ImVec2 size(buttonWidth, buttonHeight);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            bool pressed = ImGui::Button(label, size);
            ImGui::PopStyleVar();

            if (pressed)
                activeTab = id;

            style.FrameRounding = oldRounding;

            if (activeTab == id) {
                ImVec2 min = pos;
                ImVec2 max = ImVec2(pos.x + size.x, pos.y + size.y);
                float thickness = 3.0f;
                int segments = 80;

                for (int i = 0; i < segments; i++) {
                    float t0 = (float)i / segments;
                    float t1 = (float)(i + 1) / segments;
                    float shift = time * 0.5f;
                    ImVec4 col0 = RainbowColor(t0 + shift);
                    ImU32 c0 = ImGui::GetColorU32(col0);

                    ImVec2 p0, p1;
                    if (i < segments / 4) {
                        p0 = ImVec2(min.x + size.x * (t0 * 4), min.y);
                        p1 = ImVec2(min.x + size.x * (t1 * 4), min.y);
                    } else if (i < segments / 2) {
                        float tt0 = (t0 - 0.25f) * 4;
                        float tt1 = (t1 - 0.25f) * 4;
                        p0 = ImVec2(max.x, min.y + size.y * tt0);
                        p1 = ImVec2(max.x, min.y + size.y * tt1);
                    } else if (i < 3 * segments / 4) {
                        float tt0 = (t0 - 0.5f) * 4;
                        float tt1 = (t1 - 0.5f) * 4;
                        p0 = ImVec2(max.x - size.x * tt0, max.y);
                        p1 = ImVec2(max.x - size.x * tt1, max.y);
                    } else {
                        float tt0 = (t0 - 0.75f) * 4;
                        float tt1 = (t1 - 0.75f) * 4;
                        p0 = ImVec2(min.x, max.y - size.y * tt0);
                        p1 = ImVec2(min.x, max.y - size.y * tt1);
                    }

                    draw_list->AddLine(p0, p1, c0, thickness);
                }
            }
        };

        // Draw tab buttons
        for (int i = 0; i < tabCount; i++) {
            if (i > 0) ImGui::SameLine(0, spacing);
            if (i == 0) TabButton(ICON_FA_EYE " Visual", 0);
            else if (i == 1) TabButton(ICON_FA_BARS " Memory", 1);
            else if (i == 2) TabButton(ICON_FA_COGS " Setting", 2);
        }

        ImGui::Separator();

        float spacingChild = 8.0f;

        // Visual Tab
        if (activeTab == 0) {
            float halfWidth = (windowWidth - spacingChild) / 2.0f;

            ImGui::BeginChild("left", ImVec2(halfWidth, 0), true);
            ImGui::PushItemWidth(-1); // full width of child
			
            //ImGui::Checkbox("Unlimited Diamonds", &IsIntExample2);
            //Done ...now lets build it and check it
            
			ImGui::Checkbox("Ultra Vision", &Dummytoggle2);
			ImGui::Checkbox("Ghost Trail", &Dummytoggle3);
			ImGui::Checkbox("Turbo Fill", &Dummytoggle4);
			ImGui::Checkbox("Phase Shift", &Dummytoggle5);
			ImGui::Checkbox("Stealth Mode", &Dummytoggle6);
			ImGui::Checkbox("Magic Material", &Dummytoggle7);
			
            ImGui::PopItemWidth();
            ImGui::EndChild();

            ImGui::SameLine(0, spacingChild);

            ImGui::BeginChild("right", ImVec2(0, 0), true);
            ImGui::PushItemWidth(-1);
            ImGui::Checkbox("World", &Dummytoggle8);
            ImGui::Checkbox("Remap", &Dummytoggle9);
            ImGui::Checkbox("Night", &Dummytoggle10);
            ImGui::Separator();
            ImGui::TextDisabled("Other");

            static bool esp_box = false;
            static bool esp_enabled = false;
            static float esp_width = 3.4f;

            ImGui::Checkbox("ESP (3D Box)", &esp_box);
            ImGui::Checkbox("ESP", &esp_enabled);
            ImGui::SliderFloat("Width", &esp_width, 0.5f, 10.0f, "%.1f");
            ImGui::PopItemWidth();
            ImGui::EndChild();
            
            // This hacks are just dummy......you can add youe own hacks here
        }

        // Memory Tab
        if (activeTab == 1) {
            ImGui::BeginChild("memory_tab", ImVec2(0, 0), true);
            ImGui::PushItemWidth(-1);
            ImGui::Text("💾 Memory Hack 💾");
            ImGui::Checkbox("🦸Jump Hack🦸", &UnlimitedJump);
            ImGui::Checkbox("🦸Jump Hack 1🦸", &UnlimitedJump2);
            ImGui::Checkbox("🦸Jump Hack 2🦸", &UnlimitedJump3);
            ImGui::Checkbox("🦸Jump Hack 3🦸", &UnlimitedJump4);
            ImGui::Checkbox("🦸Jump Hack 4🦸", &UnlimitedJump5);
            ImGui::Checkbox("🦸Jump Hack 5🦸", &UnlimitedJump6);
            ImGui::Checkbox("⚔️PVP Damage⚔️", &PVPDMG);
            ImGui::Checkbox("🎯PVP ASIST🎯", &PVPASIST);
            ImGui::Checkbox("🦹No Collision🦹", &COLI);
            ImGui::Checkbox("🧟Kill Team🧟", &KillTeam);
            ImGui::Checkbox("🏹Attack Speed🏹", &ATTACKS);
            ImGui::Checkbox("🏹Attack Speed2🏹", &ATTACKS2);
            ImGui::Checkbox("💯Unlimited FPS💯", &UNLIFPS);
            ImGui::Checkbox("🔓Unlock High FPS🔓", &Unlock);
            ImGui::Checkbox("🧿Portal In MAP🧿", &MAP);
            ImGui::Checkbox("🥈Auto 50 LVL🥇", &LEVEL50);
            ImGui::Checkbox("👁️See Color In Chest👁️", &CHEST);
            ImGui::Checkbox("🥷See Rogue🥷", &ROGUE);
            ImGui::PopItemWidth();
            ImGui::EndChild();
        }

        // Settings Tab
        if (activeTab == 2) {
            ImGui::BeginChild("settings_tab", ImVec2(0, 0), true);
            ImGui::PushItemWidth(-1);
            ImGui::Text("Settings Page...");
            ImGui::Checkbox("Dark Mode", &Dummytoggle13);
            ImGui::Checkbox("Show FPS", &Dummytoggle14);
            ImGui::PopItemWidth();
            ImGui::EndChild();
        }

        ImGui::End();
    }
}





EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    static bool is_setup = false;
    static bool should_clear_mouse_pos = false;
    if (!is_setup) {
		SetGUI();
        is_setup = true;
    }
	
    ImGuiIO &io = ImGui::GetIO();
    int (*TouchCount)(void*) = (int (*)(void*)) (Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Input"), OBFUSCATE("get_touchCount"), 0));
    int touchCount = TouchCount(nullptr);
    if (touchCount > 0) {
        UnityEngine_Touch_Fields touch = ((UnityEngine_Touch_Fields (*)(int)) (Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), OBFUSCATE("UnityEngine"), OBFUSCATE("Input"), OBFUSCATE("GetTouch"), 1))) (0);
        float reverseY = io.DisplaySize.y - touch.m_Position.fields.y;

        switch (touch.m_Phase) {
            case TouchPhase::Began:
            case TouchPhase::Stationary:
                io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
                io.MouseDown[0] = true;
                break;
            case TouchPhase::Ended:
            case TouchPhase::Canceled:
                io.MouseDown[0] = false;
                should_clear_mouse_pos = true;
                break;
            case TouchPhase::Moved:
                io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
                break;
            default:
                break;
        }
    } else {
        io.MouseDown[0] = false;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    DrawMenu();
	ImGui::Render();
    glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
    if (should_clear_mouse_pos) {
        io.MousePos = ImVec2(-1, -1);
        should_clear_mouse_pos = false;
    }
    return old_eglSwapBuffers(dpy, surface);
}

void *hack_start(void *) {
    while (!G_IL2CPP) {
    G_IL2CPP = Tools::GetBaseAddress(targetLib);
    sleep(1);
    }
    Tools::Hook((void *) dlsym(RTLD_NEXT, "eglSwapBuffers"), (void *) hook_eglSwapBuffers, (void **) &old_eglSwapBuffers);
    Il2CppAttach();
    sleep(5);
	
	// Namespace: User
	//public class Profile 
	//public Int32 get_HardCurrency() { }
	
	
	
	//Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("User" /*Namespace*/), OBFUSCATE("Profile" /*class*/) , OBFUSCATE("get_HardCurrency" /*method*/),0 /*perameter*/),(void *) &IntExample2, (void **) &old_IntExample2);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("CharacterLogic") , OBFUSCATE("GetMaxAirJumpCount"),1),(void *) &UnliJump, (void **) &old_UnliJump);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("CharacterMovementProxy") , OBFUSCATE("get_AirJumpCount"),0),(void *) &UnliJump2, (void **) &old_UnliJump2);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("CharacterLogic") , OBFUSCATE("get_AirJumpCount"),0),(void *) &UnliJump3, (void **) &old_UnliJump3);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("CharacterLogic") , OBFUSCATE("get_ClientAirJumpCount"),0),(void *) &UnliJump4, (void **) &old_UnliJump4);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("CharacterRep") , OBFUSCATE("get_AirJumpCount"),0),(void *) &UnliJump5, (void **) &old_UnliJump5);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("CharacterLogic") , OBFUSCATE("CheckCanAirJump"),0),(void *) &UnliJump6, (void **) &old_UnliJump6);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("CharacterLogic") , OBFUSCATE("get_PVPDamage"),0),(void *) &pvp, (void **) &old_pvp);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("CharacterLogic") , OBFUSCATE("get_PVPAssistant"),0),(void *) &pvpas, (void **) &old_pvpas);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("CharacterLogic") , OBFUSCATE("GetIgnoreCollision"),1),(void *) &colli, (void **) &old_colli);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("MonsterLogic") , OBFUSCATE("GetIgnoreCollision"),1),(void *) &colli2, (void **) &old_colli2);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("Team") , OBFUSCATE("IsMember"),1),(void *) &kills, (void **) &old_kills);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("SkillLogic") , OBFUSCATE("get_ClientPlaySpeed"),1),(void *) &attack, (void **) &old_attack);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("SkillLogic") , OBFUSCATE("GetSkillPlaySpeed"),1),(void *) &attack2, (void **) &old_attack2);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("GCommon"), OBFUSCATE("QualityManager") , OBFUSCATE("SetFPSLevel"),1),(void *) &unlimitfps, (void **) &old_unlimitfps);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("GCommon"), OBFUSCATE("QualityManager") , OBFUSCATE("SupportVeryHighFPS"),0),(void *) &highfps, (void **) &old_highfps);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame"), OBFUSCATE("UIBattleMapUtils") , OBFUSCATE("JudgeIsPortalNearTeamMate"),6),(void *) &portal, (void **) &old_portal);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame"), OBFUSCATE("AccountService") , OBFUSCATE("get_PlayerLv"),0),(void *) &lvl, (void **) &old_lvl);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("ChestBoxLogic") , OBFUSCATE("get_IsOpen"),0),(void *) &seec, (void **) &old_seec);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame.Logic"), OBFUSCATE("MechanismChestLogic") , OBFUSCATE("get_IsOpen"),0),(void *) &seec1, (void **) &old_seec1);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame"), OBFUSCATE("PresentChestBox") , OBFUSCATE("IsOperationValid"),2),(void *) &seec2, (void **) &old_seec2);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame"), OBFUSCATE("PresentChestBox") , OBFUSCATE("CanOperationDisplay"),2),(void *) &seec3, (void **) &old_seec3);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame"), OBFUSCATE("PresentMechanismChest") , OBFUSCATE("IsOperationValid"),2),(void *) &seec4, (void **) &old_seec4);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame"), OBFUSCATE("PresentMechanismChest") , OBFUSCATE("CanOperationDisplay"),2),(void *) &seec5, (void **) &old_seec5);
	
	Tools::Hook(Il2CppGetMethodOffset(OBFUSCATE("KradGame.dll"), OBFUSCATE("KradGame"), OBFUSCATE("PEntity") , OBFUSCATE("ProcessEntityStealth"),0),(void *) &seerogue, (void **) &old_seerogue);
	
	
	// And here you can add your hooks
	//Note for some games the Namespace is empty...so in this case you need to leave that blank like that
	
    pthread_exit(nullptr);
    return nullptr;
}


class ImGuiModMenu : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        auto package_name = env->GetStringUTFChars(args->nice_name, nullptr);
        auto app_data_dir = env->GetStringUTFChars(args->app_data_dir, nullptr);
        preSpecialize(package_name, app_data_dir);
        env->ReleaseStringUTFChars(args->nice_name, package_name);
        env->ReleaseStringUTFChars(args->app_data_dir, app_data_dir);
    }

    void postAppSpecialize(const AppSpecializeArgs *) override {
        if (enable_hack) {
            std::thread hack_thread(hack_start, game_data_dir);
            hack_thread.detach();
        }
    }

private:
    Api *api;
    JNIEnv *env;
    bool enable_hack;
    char *game_data_dir;

    void preSpecialize(const char *package_name, const char *app_data_dir) {
        if (strcmp(package_name, targetPackageName) == 0) {
            enable_hack = true;
            game_data_dir = new char[strlen(app_data_dir) + 1];
            strcpy(game_data_dir, app_data_dir);
        }
    }
};

REGISTER_ZYGISK_MODULE(ImGuiModMenu)
