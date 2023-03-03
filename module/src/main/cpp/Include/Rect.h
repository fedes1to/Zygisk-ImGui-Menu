//
// Created by aimar on 1/13/2020.
//

#ifndef RECT_H
#define RECT_H

#include <jni.h>


class Rect {
public:
    JNIEnv *env;
    jobject rectObj;

    jfieldID leftId, topId, rightId, bottomId;

    Rect(JNIEnv *env) {
        this->env = env;

        jclass rectClass = env->FindClass("android/graphics/Rect");
        jmethodID init = env->GetMethodID(rectClass, "<init>", "()V");
        this->rectObj = env->NewGlobalRef(env->NewObject(rectClass, init));

        leftId = env->GetFieldID(rectClass, "left", "I");
        topId = env->GetFieldID(rectClass, "top", "I");
        rightId = env->GetFieldID(rectClass, "right", "I");
        bottomId = env->GetFieldID(rectClass, "bottom", "I");

        env->DeleteLocalRef(rectClass);
    }

    int getLeft();
    int getRight();
    int getTop();
    int getBottom();
    int getWidth();
    int getHeight();
};


#endif //MENU_RECT_H