#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/system_properties.h>
#include <dlfcn.h>
#include <dlfcn.h>
#include <cstdlib>
#include <cinttypes>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_android.h"
#include "KittyMemory/KittyMemory.h"
#include "KittyMemory/MemoryPatch.h"
#include "KittyMemory/KittyScanner.h"
#include "KittyMemory/KittyUtils.h"
#include "Includes/Dobby/dobbyForHooks.h"
#include "Include/Unity.h"
#include "Misc.h"
#include "hook.h"
#include "Include/Roboto-Regular.h"
#include <iostream>
#include <chrono>
#include "Include/Quaternion.h"
#include "Rect.h"
#include <fstream>
#include <limits>
#define GamePackageName "com.company.game" // define the game package name here please

int glHeight, glWidth;

// here you can define variables for the patches
bool isValidAuth, autolog;

void Pointers()
{
    /* here you can define pointers, example of a pointer (static void call):
     * addGraffiti = (void(*)()) (void*) (g_il2cppBaseMap.startAddress + string2Offset(OBFUSCATE("0x1500D2C")));
     */
}

#include "Auth.h" // this is for authentication purposes, check the header file for more info


int isGame(JNIEnv *env, jstring appDataDir)
{
    if (!appDataDir)
        return 0;
    const char *app_data_dir = env->GetStringUTFChars(appDataDir, nullptr);
    int user = 0;
    static char package_name[256];
    if (sscanf(app_data_dir, "/data/%*[^/]/%d/%s", &user, package_name) != 2) {
        if (sscanf(app_data_dir, "/data/%*[^/]/%s", package_name) != 1) {
            package_name[0] = '\0';
            LOGW(OBFUSCATE("can't parse %s"), app_data_dir);
            return 0;
        }
    }
    if (strcmp(package_name, GamePackageName) == 0) {
        LOGI(OBFUSCATE("detect game: %s"), package_name);
        game_data_dir = new char[strlen(app_data_dir) + 1];
        strcpy(game_data_dir, app_data_dir);
        env->ReleaseStringUTFChars(appDataDir, app_data_dir);
        return 1;
    } else {
        env->ReleaseStringUTFChars(appDataDir, app_data_dir);
        return 0;
    }
}


bool setupimg;

HOOKAF(void, Input, void *thiz, void *ex_ab, void *ex_ac)
{
    origInput(thiz, ex_ab, ex_ac);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
    return;
}

void Hooks() {
    // here you can define hooks, example of a hook:
    // HOOK("0x3D0000", PixelTime, old_PixelTime);
}

void Patches() {
    // here you can define patches, these can be either at runtime or whenever a bool changes, examples of both
    // PATCH_SWITCH("0x485525C", "1F2003D5C0035FD6", boolHere);
    // PATCH("0x100000", "C0035FD6");
}

bool test; // remove this whenever you're making the menu please

void DrawMenu()
{
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    {
        ImGui::Begin(OBFUSCATE("Zygisk-Menu"));
        if (isValidAuth) {
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_FittingPolicyResizeDown;
            if (ImGui::BeginTabBar("Menu", tab_bar_flags)) {
                if (ImGui::BeginTabItem(OBFUSCATE("Tab 1"))) {
                    // here menu stuff, remove test btw
                    ImGui::Checkbox(OBFUSCATE("This is a checkbox"), &test);
                    ImGui::TextUnformatted(OBFUSCATE("This is a text"));
                    if (ImGui::Button(OBFUSCATE("This is a button"))) {
                        // code for button action
                        ImGui::EndTabItem();
                    }
                }
                if (ImGui::BeginTabItem(OBFUSCATE("Tab 2"))) {
                    // example of how to do a list:
                    // ImGui::ListBox(OBFUSCATE("Select Scene"), &selectedScene, sceneList, IM_ARRAYSIZE(sceneList), 4);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            Patches();
        }
        if (!isValidAuth) {
            // parses response and shows it to the user
            ImGui::TextUnformatted(jsonresult.c_str());
        }
        ImGui::End();
    }
}

void SetupImgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float) glWidth, (float) glHeight);
    ImGui_ImplOpenGL3_Init("#version 100");
    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(7.0f);
    io.Fonts->AddFontFromMemoryTTF(Roboto_Regular, 30, 30.0f);
}


EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);

    if (!setupimg)
    {
        SetupImgui();
        setupimg = true;
    }

    if (autolog)
    {
        isValidAuth = tryAutoLogin();
        autolog = false;
    }

    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    DrawMenu();

    ImGui::EndFrame();
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return old_eglSwapBuffers(dpy, surface);
}

void *hack_thread(void *arg) {
    do {
        sleep(1);
        g_il2cppBaseMap = KittyMemory::getLibraryBaseMap("libil2cpp.so");
    } while (!g_il2cppBaseMap.isValid());
    KITTY_LOGI("il2cpp base: %p", (void*)(g_il2cppBaseMap.startAddress));

    // initialize our pointers and hooks, also we will set whether we want auth or not
    // for auth: autolog = true; and remove the line below
    isValidAuth = true;

    Pointers();
    Hooks();

    auto eglhandle = dlopen("libunity.so", RTLD_LAZY);
    auto eglSwapBuffers = dlsym(eglhandle, "eglSwapBuffers");
    DobbyHook((void*)eglSwapBuffers,(void*)hook_eglSwapBuffers,
              (void**)&old_eglSwapBuffers);
    void *sym_input = DobbySymbolResolver(("/system/lib/libinput.so"), ("_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE"));
    if (NULL != sym_input) {
        DobbyHook(sym_input,(void*)myInput,(void**)&origInput);
    }
    LOGI("Draw Done!");


    return nullptr;
}
