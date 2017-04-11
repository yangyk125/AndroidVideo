/*
 * NXWorker.cpp
 *
 *  Created on: 2015-9-1
 *      Author: yangyk
 */

#include "NXWorker.h"
#include <map>

std::map<jlong, void*> globalObjMap ;
JavaVM*				   globalJavaVM = NULL ;
JNIEnv*				   globalWchEnv = NULL ;

void* getJavaNativeObject(JNIEnv* env, jobject obj)
{
	jclass workerClazz = env->GetObjectClass(obj) ;
	jmethodID methodObject = env->GetMethodID(workerClazz, "getWorkerID", "()J");
	jlong workerID = env->CallLongMethod(obj, methodObject);
	std::map<jlong, void*>::iterator it = globalObjMap.find(workerID) ;
	if(it == globalObjMap.end())
		return NULL ;
	return it->second ;
}

void setJavaNativeObject(JNIEnv* env, jobject obj, void* value)
{
	jclass workerClazz = env->GetObjectClass(obj) ;
	jmethodID methodObject = env->GetMethodID(workerClazz, "getWorkerID", "()J");
	jlong workerID = env->CallLongMethod(obj, methodObject);
	globalObjMap[workerID] = value ;
}

void  delJavaNativeObject(JNIEnv* env, jobject obj)
{
	jclass workerClazz = env->GetObjectClass(obj) ;
	jmethodID methodObject = env->GetMethodID(workerClazz, "getWorkerID", "()J");
	jlong workerID = env->CallLongMethod(obj, methodObject);
	std::map<jlong, void*>::iterator it = globalObjMap.find(workerID) ;
	if(it != globalObjMap.end())
		globalObjMap.erase(it);
}

JavaVM* getGlobalJavaVM()
{
	return globalJavaVM ;
}

JNIEnv* getGlobalWchEnv()
{
	if(globalWchEnv == NULL)
		globalJavaVM->AttachCurrentThread(&globalWchEnv, NULL) ;
	return globalWchEnv ;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	globalJavaVM = vm ;
	return JNI_VERSION_1_4 ;
}
