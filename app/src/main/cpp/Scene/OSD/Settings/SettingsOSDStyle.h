//
// Created by Constantin on 2/3/2019.
//

#ifndef FPV_VR_SETTINGSOSDSTYLE_H
#define FPV_VR_SETTINGSOSDSTYLE_H


#include <jni.h>
#include <TrueColor.hpp>
#include <TextAssetsHelper.hpp>

class SettingsOSDStyle {
public:
    SettingsOSDStyle(JNIEnv *env, jobject androidContext);
    SettingsOSDStyle(SettingsOSDStyle const &) = delete;
    void operator=(SettingsOSDStyle const &)= delete;
    static TrueColor getOSDBackgroundColor(int _alpha){
        float alpha=_alpha*0.01f;
        if(alpha<0 || alpha>1){alpha=0;}
        return TrueColor(glm::vec4(0.0f, 0.0f, 0.0f, alpha));
    }
    static bool isTransparentBackgroundEnabled(int _alpha){
        return _alpha>0;
    }
public:
    //----STYLE---
    int OSD_MONO_LINE_WIDTH;
    int OSD_STEREO_LINE_WIDTH;
    TrueColor OSD_LINE_FILL_COLOR;
    TrueColor OSD_LINE_OUTLINE_COLOR;
    //
    //int OSD_TEXT_FONT_TYPE;
    TextAssetsHelper::TEXT_STYLE  OSD_TEXT_FONT_TYPE;
    TrueColor OSD_TEXT_FILL_COLOR1;
    TrueColor OSD_TEXT_FILL_COLOR2;
    TrueColor OSD_TEXT_FILL_COLOR3;
    TrueColor OSD_TEXT_OUTLINE_COLOR;
    float OSD_MONO_TEXT_OUTLINE_STRENGTH;
    float OSD_STEREO_TEXT_OUTLINE_STRENGTH;
    int OSD_TRANSPARENT_BACKGROUND_STRENGTH;
    //
    int OSD_MONO_GLOBAL_SCALE;
    int OSD_STEREO_GLOBAL_SCALE;
public:
    float OSD_STEREO_RATIO;
};


#endif //FPV_VR_SETTINGSOSDSTYLE_H
