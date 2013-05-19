#include <jni.h>

#include "uSynergy.h"

extern uSynergyContext uSynergyLinuxContext;

/*
 * init() Initialization
 * jint  0:success 1:faild
 */
jint java_io_brotherhood_usynergy_MainActivity_init(JNIEnv *env, jobject thiz)
{
	uSynergyInit(&uSynergyLinuxContextm, "Android", 1024, 600);
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
jint java_io_brotherhood_usynergy_MainActivity_shutdown(JNIEnv *env,
	jobject thiz)
{
	uSynergyStop(&uSynergyLinuxContext);
}

/*
 * setClientName() set Client Name
 * jint  0:success 1:faild
 */
jint java_io_brotherhood_usynergy_MainActivity_setClientName(JNIEnv *env,
	jobject thiz, jstring clientName)
{
	uSynergySetClientName(&uSynergyLinuxContext, clientName);
}

/*
 * setServerName() set Sever Name
 */
jint java_io_brotherhood_usynergy_MainActivity_setServerName(JNIEnv *env,
	jobject thiz, jstring serverName)
{
	uSynergySetServerName(&uSynergyLinuxContext, serverName);
}

/*
 * setIp() set IP address
 * jint  0:success 1:faild
 */
jint java_io_brotherhood_usynergy_MainActivity_setSeverIp(JNIEnv *env,
	jobject thiz, jstring ipAdd)
{

}

jint java_io_brotherhood_usynergy_MainActivity_exit(JNIEnv *env)
{
	uSynergCleanUP(&uSynergyLinuxContext);
}
