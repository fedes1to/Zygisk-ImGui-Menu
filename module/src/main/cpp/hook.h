
#ifndef ZygiskImGui_HOOK_H
#define ZygiskImGui_HOOK_H

#include <jni.h>

static int enable_hack;
static char *game_data_dir = NULL;

int isGame(JNIEnv *env, jstring appDataDir);

void *hack_thread(void *arg);

#include <android/log.h>

#define LOG_TAG "zyCheats"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#define HOOKAF(ret, func, ...) \
    ret (*orig##func)(__VA_ARGS__); \
    ret my##func(__VA_ARGS__)

#endif //ZygiskImGui_HOOK_H