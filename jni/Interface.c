#include <jni.h>
#include <android/log.h>
#include "uSynergy.h"

extern uSynergyContext uSynergyLinuxContext;

/*
 * init() Initialization
 * jint  0:success 1:faild
 */
jint Java_io_brotherhood_usynergy_service_UsynergyService_init(JNIEnv *env, jobject thiz,jstring screenName,jint hight,jint width)
{
	__android_log_print(ANDROID_LOG_INFO,"interface","init = %s:%d*%d",screenName, hight, width);
	uSynergyInit(&uSynergyLinuxContext, screenName, hight, width);
}
/**
 * getclipBoard Text
 * jstring clipboard Text
 */
jstring Java_io_brotherhood_usynergy_service_UsynergyService_getClipBoardText(JNIEnv *env,jobject thiz){

//	return null;
}

/*
 * start()
 * jint  0:success 1:faild
 */
jint Java_io_brotherhood_usynergy_service_UsynergyService_start(JNIEnv *env, jobject thiz,jstring ip,jint port)
{
	uSynergyStart(&uSynergyLinuxContext,ip,port);
	__android_log_print(ANDROID_LOG_INFO,"interface","start = %s:%d",ip,port);
//	uSynergyStart(&uSynergyLinuxContext, "192.168.0.102", 24800);
	//uSynergyStart(&uSynergyLinuxContext, "10.11.71.173", 24800);
}

/*
 * shutdown()
 * jint  0:success 1:faild
 */
jint Java_io_brotherhood_usynergy_service_UsynergyService_shutdown(JNIEnv *env,jobject thiz)
{
	uSynergyStop(&uSynergyLinuxContext);
}

/*
 * setClientName() set Client Name
 * jint  0:success 1:faild
 */
jint Java_io_brotherhood_usynergy_service_UsynergyService_setClientName(JNIEnv *env,jobject thiz, jstring clientName,jint height,jint width)
{
	uSynergCleanUP(&uSynergyLinuxContext);
//	uSynergyInit(&uSynergyLinuxContext, clientName, 1024, 600);
	uSynergyInit(&uSynergyLinuxContext, clientName, height, width);
}

jint Java_io_brotherhood_usynergy_service_UsynergyService_exit(JNIEnv *env)
{
	uSynergCleanUP(&uSynergyLinuxContext);
}
