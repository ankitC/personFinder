LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED
include D:\\Android\\Projects\\OpenCV\\OpenCV-2.4.5-android-sdk\\sdk\\native\\jni\\OpenCV.mk

#include ../../sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES  := Detector_jni.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_LDLIBS     += -llog -ldl

LOCAL_MODULE     := detector

include $(BUILD_SHARED_LIBRARY)