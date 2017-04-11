/*
 * NXWorker.h
 *
 *  Created on: 2015-9-1
 *      Author: yangyk
 */

#include <jni.h>
#include <stdint.h>
#include <android/log.h>

void* getJavaNativeObject(JNIEnv* env, jobject obj);

void  setJavaNativeObject(JNIEnv* env, jobject obj, void* value) ;

void  delJavaNativeObject(JNIEnv* env, jobject obj);

JavaVM* getGlobalJavaVM() ;

JNIEnv* getGlobalWchEnv() ;
