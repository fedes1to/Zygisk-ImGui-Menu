#ifndef JNISTUFF
#define JNISTUFF

#include <jni.h>
JavaVM *jvm;
jclass UnityPlayer_cls;
jfieldID UnityPlayer_CurrentActivity_fid;
JNIEnv* getEnv() {
    JNIEnv *env;
    int status = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if(status < 0) {
        status = jvm->AttachCurrentThread(&env, NULL);
        if(status < 0) {
            LOGE(OBFUSCATE("Error Getting JNI"), 1);
            return nullptr;
        }
    }
    return env;
}
jobject getGlobalContext(JNIEnv *env)
{
    jclass activityThread = env->FindClass(OBFUSCATE("android/app/ActivityThread"));
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThread, OBFUSCATE("currentActivityThread"), OBFUSCATE("()Landroid/app/ActivityThread;"));
    jobject at = env->CallStaticObjectMethod(activityThread, currentActivityThread);
    jmethodID getApplication = env->GetMethodID(activityThread, OBFUSCATE("getApplication"), OBFUSCATE("()Landroid/app/Application;"));
    jobject context = env->CallObjectMethod(at, getApplication);
    return context;
}
#endif JNISTUFF