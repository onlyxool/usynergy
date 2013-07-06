#include <jni.h>

#include "uSynergy.h"

extern uSynergyContext uSynergyLinuxContext;

/*
 * init() Initialization
 * jint  0:success 1:faild
 */
jint java_io_brotherhood_usynergy_MainActivity_init(JNIEnv *env, jobject thiz)
{
	uSynergyInit(&uSynergyLinuxContext, "Android", 1024, 600);
}

/*
 * start()
 * jint  0:success 1:faild
 */
jint java_io_brotherhood_usynergy_MainActivity_start(JNIEnv *env, jobject thiz)
{
	uSynergyStart(&uSynergyLinuxContext, "192.168.0.102", 24800);
	//uSynergyStart(&uSynergyLinuxContext, "10.11.71.173", 24800);
}

/*
 * shutdown()
 * jint  0:success 1:faild
 */
jint java_io_brotherhood_usynergy_MainActivity_shutdown(JNIEnv *env,jobject thiz)
{
	uSynergyStop(&uSynergyLinuxContext);
}

/*
 * setClientName() set Client Name
 * jint  0:success 1:faild
 */
jint java_io_brotherhood_usynergy_MainActivity_setClientName(JNIEnv *env,jobject thiz, jstring clientName)
{
	uSynergCleanUP(&uSynergyLinuxContext);
	uSynergyInit(&uSynergyLinuxContext, clientName, 1024, 600);
}

jint java_io_brotherhood_usynergy_MainActivity_exit(JNIEnv *env)
{
	uSynergCleanUP(&uSynergyLinuxContext);
}
