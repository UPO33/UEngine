#pragma once


#include <jni.h>
#include <errno.h>

#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

#include <EGL/egl.h>

#ifdef USE_GLES1
#include <GLES/gl.h>
#else
#include <GLES2/gl2.h>
#endif


#include <android/sensor.h>
#include <android/log.h>