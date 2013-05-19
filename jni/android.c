#include <string.h>

#include "platform.h"
#include "suinput.h"

static void uSynergyUpdateServer(uSynergyCookie cookie, char *addr, int port)
{
	memset(&(cookie->server_addr), 0, sizeof(struct sockaddr));
	cookie->server_addr.sin_family = AF_INET;
	cookie->server_addr.sin_port = htons(port);
	cookie->server_addr.sin_addr.s_addr = inet_addr(addr);
}

static uSynergyBool uSynergyConnectFunc(uSynergyCookie cookie)
{
	cookie->sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(cookie->sockfd, (struct sockaddr *)& cookie->server_addr,
		sizeof(struct sockaddr)) == 0) {
		return USYNERGY_TRUE;
	} else {
		perror("connect error:");
		return USYNERGY_FALSE;
	}
}

static uSynergyBool uSynergyReceiveFunc(uSynergyCookie cookie, uint8_t *buffer,
	int maxLength, int* outLength)
{
	int ret;
	ret = recv(cookie->sockfd, buffer, maxLength, 0);
	if (ret <= 0) {
		perror("receive error:");
		return USYNERGY_FALSE;
	}

	*outLength = ret;
	return USYNERGY_TRUE;
}

static uSynergyBool uSynergySendFunc(uSynergyCookie cookie,
	const uint8_t *buffer, int length)
{
	int ret;
	ret = send(cookie->sockfd, buffer, length, 0);
	if (ret < 0 || ret != length)
		return USYNERGY_FALSE;

	return USYNERGY_TRUE;
}

static uint32_t uSynergyGetTimeFunc()
{
	uint32_t ret = 0;
	time_t now;

	ret = time(&now);

	return ret*1000;
}

#define BUS_VIRTUAL 0x06
static uSynergyBool uSynergyConnectDevice(uSynergyCookie cookie)
{
	struct input_id device_id;
	device_id.bustype = BUS_VIRTUAL;
	device_id.vendor  = 1;
	device_id.product = 1;
	device_id.version = 0x0100;

	cookie->uinput_keyboard = suinput_open("qwerty", &(cookie->device_id),
		keyboard);
	cookie->uinput_mouse = suinput_open("synergy-mouse", &(cookie->device_id),
		mouse);

	return USYNERGY_TRUE;
}

static void uSynergyDisconnectDevice(uSynergyCookie cookie)
{
	suinput_close(cookie->uinput_mouse);
	suinput_close(cookie->uinput_keyboard);
}

static void uSynergyScreenActiveCallback(uSynergyCookie cookie,
	uSynergyBool active)
{
	if (active) {
		/* Screen active */
	} else {

	}
}

static uSynergyBool uSynergyMouseMoveCallback(uSynergyCookie cookie,
	int32_t x, int32_t y)
{
	int ret;
	ret = suinput_move_pointer(cookie->uinput_mouse, x, y);
	return USYNERGY_TRUE;
}

static uSynergyBool uSynergyMouseUpCallback(uSynergyCookie cookie,
	uSynergyBool buttonLeft, uSynergyBool buttonRight, uSynergyBool buttonMiddle)
{
	int ret;
	if (!buttonLeft)
		ret = suinput_release(cookie->uinput_mouse, BTN_LEFT);

	if (!buttonRight)
		ret = suinput_release(cookie->uinput_mouse, BTN_RIGHT);

	if (!buttonMiddle)
		ret = suinput_release(cookie->uinput_mouse, BTN_MIDDLE);

	return USYNERGY_TRUE;
}

static uSynergyBool uSynergyMouseDownCallback(uSynergyCookie cookie,
	uSynergyBool buttonLeft, uSynergyBool buttonRight, uSynergyBool buttonMiddle)
{
	int ret;
	if (buttonLeft)
		ret = suinput_press(cookie->uinput_mouse, BTN_LEFT);

	if (buttonRight)
		ret = suinput_press(cookie->uinput_mouse, BTN_RIGHT);

	if (buttonMiddle)
		ret = suinput_press(cookie->uinput_mouse, BTN_MIDDLE);

	return USYNERGY_TRUE;
}

static uSynergyBool uSynergyMouseWheelCallback(uSynergyCookie cookie,
	int16_t wheelX, int16_t wheelY)
{
	return USYNERGY_TRUE;
}

static void uSynergyKeyboardCallback(uSynergyCookie cookie, uint16_t key,
	uint16_t modifiers, uSynergyBool down, uSynergyBool repeat)
{
	int ret;
	if (down)
		ret = suinput_press(cookie->uinput_keyboard, key);
	else
		ret = suinput_release(cookie->uinput_keyboard, key);
}

#define msleep(n) usleep(n*1000)
static void uSynergySleepFunc(uSynergyCookie cookie, int timeMs)
{
	msleep(timeMs);
}

uSynergyContext uSynergyLinuxContext = {
	.m_updateServerAddr	= uSynergyUpdateServer,
	.m_connectFunc      = uSynergyConnectFunc,
	.m_receiveFunc      = uSynergyReceiveFunc,
	.m_sendFunc         = uSynergySendFunc,
	.m_getTimeFunc      = uSynergyGetTimeFunc,
	.m_connectDevice    = uSynergyConnectDevice,
	.m_disconnectDevice	= uSynergyDisconnectDevice,
	.m_screenActiveCallback = uSynergyScreenActiveCallback,
	.m_mouseMoveCallback    = uSynergyMouseMoveCallback,
	.m_mouseUpCallback  = uSynergyMouseUpCallback,
	.m_mouseDownCallback= uSynergyMouseDownCallback,
	.m_mouseWheelCallback   = uSynergyMouseWheelCallback,
	.m_keyboardCallback = uSynergyKeyboardCallback,
	.m_sleepFunc        = uSynergySleepFunc,
	.m_traceFunc		= NULL,
	.m_joystickCallback = NULL,
	.m_clipboardCallback= NULL,
};
