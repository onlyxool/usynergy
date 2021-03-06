/*
 * uSynergy client -- Implementation for the embedded Synergy client library
 * version 1.0.0, July 7th, 2012

 * Copyright (c) 2012 Alex Evans

 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.

 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:

 *  1. The origin of this software must not be misrepresented; you must not
 *  claim that you wrote the original software. If you use this software
 *  in a product, an acknowledgment in the product documentation would be
 *  appreciated but is not required.

 *  2. Altered source versions must be plainly marked as such, and must not be
 *  misrepresented as being the original software.

 *  3. This notice may not be removed or altered from any source
 *  distribution.
 */

#include <stdio.h>
#include <string.h>

#include "uSynergy.h"
#include "keymap.h"
#include "log.h"

//-----------------------------------------------------------------------------
//	Internal helpers
//-----------------------------------------------------------------------------

/*
 * @brief Read 16 bit integer in network byte order and convert to native byte
 * order
 */
static int16_t sNetToNative16(const unsigned char *value)
{
#ifdef USYNERGY_LITTLE_ENDIAN
	return value[1] | (value[0] << 8);
#else
	return value[0] | (value[1] << 8);
#endif
}

/*
 * @brief Read 32 bit integer in network byte order and convert to native byte
 * order
 */
static int32_t sNetToNative32(const unsigned char *value)
{
#ifdef USYNERGY_LITTLE_ENDIAN
	return value[3] | (value[2] << 8) | (value[1] << 16) | (value[0] << 24);
#else
	return value[0] | (value[1] << 8) | (value[2] << 16) | (value[3] << 24);
#endif
}

/*
 * @brief Trace text to client
 */
static void sTrace(uSynergyContext *context, const char* text)
{
	// Don't trace if we don't have a trace function
	if (context->m_traceFunc != NULL)
		context->m_traceFunc(context->m_cookie, text);
}

/*
 * @brief Add string to reply packet
 */
static void sAddString(uSynergyContext *context, const char *string)
{
	size_t len = strlen(string);
	memcpy(context->m_replyCur, string, len);
	context->m_replyCur += len;
}

/*
 * @brief Add uint8 to reply packet
 */
static void sAddUInt8(uSynergyContext *context, uint8_t value)
{
	*context->m_replyCur++ = value;
}

/*
 * @brief Add uint16 to reply packet
 */
static void sAddUInt16(uSynergyContext *context, uint16_t value)
{
	uint8_t *reply = context->m_replyCur;
	*reply++ = (uint8_t)(value >> 8);
	*reply++ = (uint8_t)value;
	context->m_replyCur = reply;
}

/*
 * @brief Add uint32 to reply packet
 */
static void sAddUInt32(uSynergyContext *context, uint32_t value)
{
	uint8_t *reply = context->m_replyCur;

	*reply++ = (uint8_t)(value >> 24);
	*reply++ = (uint8_t)(value >> 16);
	*reply++ = (uint8_t)(value >> 8);
	*reply++ = (uint8_t)value;
	context->m_replyCur = reply;
}

/*
 * @brief Mark context as being disconnected
 */
static void sSetDisconnected(uSynergyContext *context)
{
	context->m_connected		= USYNERGY_FALSE;
	context->m_hasReceivedHello	= USYNERGY_FALSE;
	context->m_isCaptured		= USYNERGY_FALSE;
	context->m_replyCur			= context->m_replyBuffer + 4;
	context->m_sequenceNumber	= 0;
	context->m_disconnectDevice(context->m_cookie);
}

/*
 * @brief Send reply packet
 */
static uSynergyBool sSendReply(uSynergyContext *context)
{
	// Set header size
	uint8_t	*reply_buf = context->m_replyBuffer;
	/* Total size of reply */
	uint32_t reply_len = (uint32_t)(context->m_replyCur - reply_buf);
	/* Size of body */
	uint32_t body_len = reply_len - 4;
	uSynergyBool ret;

	reply_buf[0] = (uint8_t)(body_len >> 24);
	reply_buf[1] = (uint8_t)(body_len >> 16);
	reply_buf[2] = (uint8_t)(body_len >> 8);
	reply_buf[3] = (uint8_t)body_len;

	// Send reply
	ret = context->m_sendFunc(context->m_cookie,
		context->m_replyBuffer, reply_len);

	// Reset reply buffer write pointer
	context->m_replyCur = context->m_replyBuffer+4;
	return ret;
}

/*
 * @brief Call mouse callback after a mouse event
 */
static void sSendMouseMoveCallback(uSynergyContext *context,
	int32_t x, int32_t y)
{
	if (context->m_mouseMoveCallback == NULL)
		return;

	if (x == context->m_mouseX && y == context->m_mouseY)
		return;

	uSynergyBool ret;
	ret = context->m_mouseMoveCallback(context->m_cookie,
		x - context->m_mouseX, y - context->m_mouseY);
	if (!ret) {
		//LOGI("%d:%d -> %d:%d\n", context->m_mouseX_old,
		//	context->m_mouseY_old, context->m_mouseX, context->m_mouseY);
		//printf("%d:%d -> %d:%d\n", context->m_mouseX_old,context->m_mouseY_old, context->m_mouseX, context->m_mouseY);
		context->m_mouseX = x;
		context->m_mouseY = y;
	} else {
		//printf("iiiiiiiii %d ooooooo  %d:%d -> %d:%d\n",ret, context->m_mouseX_old,
		//	context->m_mouseY_old, context->m_mouseX, context->m_mouseY);
		//LOGI("iiiiiiiii %d ooooooo  %d:%d -> %d:%d\n", ret, context->m_mouseX_old,
		//	context->m_mouseY_old, context->m_mouseX, context->m_mouseY);
	}
}

static void sSendMouseUpCallback(uSynergyContext *context)
{
	if (context->m_mouseUpCallback == NULL)
		return;

	uSynergyBool ret;
	ret = context->m_mouseUpCallback(context->m_cookie,
		context->m_mouseButtonLeft, context->m_mouseButtonRight,
		context->m_mouseButtonMiddle);
}

static void sSendMouseDownCallback(uSynergyContext *context)
{
	if (context->m_mouseDownCallback == NULL)
		return;

	uSynergyBool ret;
	ret = context->m_mouseDownCallback(context->m_cookie,
		context->m_mouseButtonLeft, context->m_mouseButtonRight,
		context->m_mouseButtonMiddle);
}

static void sSendMouseWheelCallback(uSynergyContext *context)
{
	if (context->m_mouseWheelCallback == NULL)
		return;

	uSynergyBool ret;
	ret = context->m_mouseWheelCallback(context->m_cookie,
		context->m_mouseWheelX, context->m_mouseWheelY);
}

/*
 * @brief Send keyboard callback when a key has been pressed or released
 */
static void sSendKeyboardCallback(uSynergyContext *context, uint16_t key,
	uint16_t modifiers, uSynergyBool down, uSynergyBool repeat)
{
	// Skip if no callback is installed
	if (context->m_keyboardCallback == NULL)
		return;

	// Send callback
	context->m_keyboardCallback(context->m_cookie, key,
		modifiers, down, repeat);
}

/*
 * @brief Send joystick callback
 */
static void sSendJoystickCallback(uSynergyContext *context, uint8_t joyNum)
{
	int8_t *sticks;

	// Skip if no callback is installed
	if (context->m_joystickCallback == NULL)
		return;

	// Send callback
	sticks = context->m_joystickSticks[joyNum];
	context->m_joystickCallback(context->m_cookie, joyNum,
		context->m_joystickButtons[joyNum], sticks[0], sticks[1],
		sticks[2], sticks[3]);
}

/*
 * @brief Parse a single client message, update state, send callbacks
 *  and send replies
 */
#define USYNERGY_IS_PACKET(pkt_id)	memcmp(message+4, pkt_id, 4)==0
static void sProcessMessage(uSynergyContext *context, const uint8_t *message)
{
	// We have a packet!
	if (memcmp(message+4, "Synergy", 7)==0) {
		// Welcome message
		// kMsgHello = "Synergy%2i%2i"
		// kMsgHelloBack = "Synergy%2i%2i%s"
		sAddString(context, "Synergy");
		sAddUInt16(context, USYNERGY_PROTOCOL_MAJOR);
		sAddUInt16(context, USYNERGY_PROTOCOL_MINOR);
		sAddUInt32(context, (uint32_t)strlen(context->m_clientName));
		sAddString(context, context->m_clientName);

		if (!sSendReply(context)) {
			// Send reply failed, let's try to reconnect
			sTrace(context, "SendReply failed, trying to reconnect in a second");
			context->m_connected = USYNERGY_FALSE;
			context->m_sleepFunc(context->m_cookie, 1000);
		} else {
			// Let's assume we're connected
			char buffer[256+1];
			sprintf(buffer, "Connected as client \"%s\"", context->m_clientName);
			sTrace(context, buffer);
			context->m_hasReceivedHello = USYNERGY_TRUE;
			context->m_lastMessageTime = context->m_getTimeFunc();
		}
		return;

	} else if (USYNERGY_IS_PACKET("QINF")) {
		// Screen info. Reply with DINF
		// kMsgQInfo = "QINF"
		// kMsgDInfo = "DINF%2i%2i%2i%2i%2i%2i%2i"
		uint16_t x = 0, y = 0, warp = 0;
		sAddString(context, "DINF");
		sAddUInt16(context, x);
		sAddUInt16(context, y);
		sAddUInt16(context, context->m_clientWidth);
		sAddUInt16(context, context->m_clientHeight);
		sAddUInt16(context, warp);
		sAddUInt16(context, 0); // mx?
		sAddUInt16(context, 0); // my?
		sSendReply(context);
		return;

	} else if (USYNERGY_IS_PACKET("CIAK")) {
		// Do nothing?
		// kMsgCInfoAck = "CIAK"
		return;

	} else if (USYNERGY_IS_PACKET("CROP")) {
		// Do nothing?
		// kMsgCResetOptions = "CROP"
		return;

	} else if (USYNERGY_IS_PACKET("CINN")) {
		// Screen enter. Reply with CNOP
		// kMsgCEnter = "CINN%2i%2i%4i%2i"
		context->m_mouseX_old = sNetToNative16(message + 8);
		context->m_mouseY_old = sNetToNative16(message + 10);

		// Obtain the Synergy sequence number
		context->m_sequenceNumber = sNetToNative32(message + 12);
		context->m_isCaptured = USYNERGY_TRUE;

		// Call callback
		if (context->m_screenActiveCallback != NULL)
			context->m_screenActiveCallback(context->m_cookie, USYNERGY_TRUE);

	} else if (USYNERGY_IS_PACKET("COUT")) {
		// Screen leave
		// kMsgCLeave = "COUT"
		context->m_isCaptured = USYNERGY_FALSE;

		// Call callback
		if (context->m_screenActiveCallback != NULL)
			context->m_screenActiveCallback(context->m_cookie, USYNERGY_FALSE);

	} else if (USYNERGY_IS_PACKET("DMDN")) {
		// Mouse down
		// kMsgDMouseDown = "DMDN%1i"
		char btn = message[8]-1;
		if (btn==2)
			context->m_mouseButtonRight		= USYNERGY_TRUE;
		else if (btn==1)
			context->m_mouseButtonMiddle	= USYNERGY_TRUE;
		else
			context->m_mouseButtonLeft		= USYNERGY_TRUE;
		sSendMouseDownCallback(context);

	} else if (USYNERGY_IS_PACKET("DMUP")) {
		// Mouse up
		// kMsgDMouseUp = "DMUP%1i"
		char btn = message[8]-1;
		if (btn==2)
			context->m_mouseButtonRight		= USYNERGY_FALSE;
		else if (btn==1)
			context->m_mouseButtonMiddle	= USYNERGY_FALSE;
		else
			context->m_mouseButtonLeft		= USYNERGY_FALSE;
		sSendMouseUpCallback(context);

	} else if (USYNERGY_IS_PACKET("DMMV")) {
		// Mouse move. Reply with CNOP
		// kMsgDMouseMove = "DMMV%2i%2i"
		sSendMouseMoveCallback(context, sNetToNative16(message+8), sNetToNative16(message+10));

	} else if (USYNERGY_IS_PACKET("DMWM")) {
		// Mouse wheel
		// kMsgDMouseWheel = "DMWM%2i%2i"
		// kMsgDMouseWheel1_0 = "DMWM%2i"
		context->m_mouseWheelX += sNetToNative16(message+8);
		context->m_mouseWheelY += sNetToNative16(message+10);
		sSendMouseWheelCallback(context);

	} else if (USYNERGY_IS_PACKET("DKDN")) {
		// Key down
		// kMsgDKeyDown	= "DKDN%2i%2i%2i"
		// kMsgDKeyDown1_0 = "DKDN%2i%2i"
		uint16_t id = sNetToNative16(message+8);
		uint16_t mod = sNetToNative16(message+10);
		uint16_t key = sNetToNative16(message+12);
		//LOGI("id:%d key:%d mod:%d\n", id, key, mod);
		sSendKeyboardCallback(context, keyTranslation[id], mod, USYNERGY_TRUE, USYNERGY_FALSE);

	} else if (USYNERGY_IS_PACKET("DKRP")) {
		// Key repeat
		// kMsgDKeyRepeat = "DKRP%2i%2i%2i%2i"
		// kMsgDKeyRepeat1_0 = "DKRP%2i%2i%2i"
		uint16_t id = sNetToNative16(message+8);
		uint16_t mod = sNetToNative16(message+10);
//		uint16_t count = sNetToNative16(message+12);
		uint16_t key = sNetToNative16(message+14);
		sSendKeyboardCallback(context, keyTranslation[id], mod, USYNERGY_TRUE, USYNERGY_TRUE);

	} else if (USYNERGY_IS_PACKET("DKUP")) {
		// Key up
		// kMsgDKeyUp = "DKUP%2i%2i%2i"
		// kMsgDKeyUp1_0 = "DKUP%2i%2i"
		uint16_t id = sNetToNative16(message+8);
		uint16_t mod = sNetToNative16(message+10);
		uint16_t key = sNetToNative16(message+12);
		sSendKeyboardCallback(context, keyTranslation[id], mod, USYNERGY_FALSE, USYNERGY_FALSE);

	} else if (USYNERGY_IS_PACKET("DGBT")) {
		// Joystick buttons
		// kMsgDGameButtons = "DGBT%1i%2i";
		uint8_t	joy_num = message[8];
		if (joy_num<USYNERGY_NUM_JOYSTICKS) {
			// Copy button state, then send callback
			context->m_joystickButtons[joy_num] = (message[9] << 8) | message[10];
			sSendJoystickCallback(context, joy_num);
		}

	} else if (USYNERGY_IS_PACKET("DGST")) {
		// Joystick sticks
		// kMsgDGameSticks = "DGST%1i%1i%1i%1i%1i";
		uint8_t	joy_num = message[8];
		if (joy_num<USYNERGY_NUM_JOYSTICKS) {
			// Copy stick state, then send callback
			memcpy(context->m_joystickSticks[joy_num], message+9, 4);
			sSendJoystickCallback(context, joy_num);
		}

	} else if (USYNERGY_IS_PACKET("DSOP")) {
		// Set options
		// kMsgDSetOptions = "DSOP%4I"

	} else if (USYNERGY_IS_PACKET("CALV")) {
		// Keepalive, reply with CALV and then CNOP
		// kMsgCKeepAlive = "CALV"
		sAddString(context, "CALV");
		sSendReply(context);
		// now reply with CNOP

		// Update timer
		context->m_lastMessageTime = context->m_getTimeFunc();

	} else if (USYNERGY_IS_PACKET("DCLP")) {
		/* Clipboard message
		 * kMsgDClipboard = "DCLP%1i%4i%s"

		 * The clipboard message contains:
		 *	1 uint32:	The size of the message
		 *	4 chars: 	The identifier ("DCLP")
		 *	1 uint8: 	The clipboard index
		 *	1 uint32:	The sequence number. It's zero, because this message is
		 *              always coming from the server?
		 *	1 uint32:	The total size of the remaining 'string' (as per the
		 *				Synergy %s string format (which is 1 uint32 for size 
		 *				followed by a char buffer (not necessarily null terminated)).
		 *	1 uint32:	The number of formats present in the message
		 * And then 'number of formats' times the following:
		 *	1 uint32:	The format of the clipboard data
		 *	1 uint32:	The size n of the clipboard data
		 *	n uint8:	The clipboard data
		 */
		const uint8_t *	parse_msg	= message+17;
		uint32_t		num_formats = sNetToNative32(parse_msg);
		parse_msg += 4;
		for (; num_formats; num_formats--) {
			// Parse clipboard format header
			uint32_t format	= sNetToNative32(parse_msg);
			uint32_t size	= sNetToNative32(parse_msg+4);
			parse_msg += 8;

			// Call callback
			if (context->m_clipboardCallback)
				context->m_clipboardCallback(context->m_cookie, format,
					parse_msg, size);

			parse_msg += size;
		}
	} else if (USYNERGY_IS_PACKET("EUNK") || USYNERGY_IS_PACKET("EBAD")) {
		/* kMsgEUnknown = "EUNK" kMsgEBad = "EBAD" */
		char buffer[64];
		sprintf(buffer, "Unknow client, please add a client \"%s\" on your \
			synergy server.\n", context->m_clientName);
		sTrace(context, buffer);
		sSetDisconnected(context);
		return;
	} else {
		/* Unknown packet, could be any of these
		 *		kMsgCNoop 			= "CNOP"
		 *		kMsgCClose 			= "CBYE"
		 *		kMsgCClipboard 		= "CCLP%1i%4i"
		 *		kMsgCScreenSaver 	= "CSEC%1i"
		 *		kMsgDKeyRepeat		= "DKRP%2i%2i%2i%2i"
		 *		kMsgDKeyRepeat1_0	= "DKRP%2i%2i%2i"
		 *		kMsgDMouseRelMove	= "DMRM%2i%2i"
		 *		kMsgEIncompatible	= "EICV%2i%2i"
		 *		kMsgEBusy 			= "EBSY"
		 */
		char buffer[64];
		sprintf(buffer, "Unknown packet '%c%c%c%c'", message[4], message[5],
			message[6], message[7]);
		LOGI("Unknown packet '%c%c%c%c'\n", message[4], message[5],
			message[6], message[7]);
		sTrace(context, buffer);
		return;
	}

	// Reply with CNOP maybe?
	sAddString(context, "CNOP");
	sSendReply(context);
}
#undef USYNERGY_IS_PACKET

void *sRecvData(void *arg)
{
	/* Receive data (blocking) */
	int receive_size;
	int num_received = 0;
	uint8_t *packhead = NULL;
	uint8_t netRecvBuffer[USYNERGY_NETRECV_BUFFER_SIZE];
	int packlen, recvSumLen, netrecvOfs = 0;
	uSynergyContext *context = arg;

	while (context->m_connected) {
		receive_size = USYNERGY_NETRECV_BUFFER_SIZE - netrecvOfs;
		memset(netRecvBuffer + netrecvOfs, 0, receive_size);

		if (context->m_receiveFunc(context->m_cookie, netRecvBuffer + netrecvOfs,
			receive_size, &num_received) == USYNERGY_FALSE) {
			/* Receive failed, let's try to reconnect */
			char buffer[128];
			sprintf(buffer, "Receive failed (%d bytes asked, %d bytes received), \
				trying to reconnect in a second", receive_size, num_received);
			sTrace(context, buffer);
			sSetDisconnected(context);
			sem_post(&context->reciveOfsSem);
			context->m_sleepFunc(context->m_cookie, 100);
			return;
		}

		pthread_mutex_lock(&context->m_receiveMutex);
		packhead = netRecvBuffer;
		packlen = sNetToNative32(packhead);
		recvSumLen = packlen + 4;

		while (1) {
//LOGI("packlen::%d %c%c%c%c\n", packlen, *(packhead+4), *(packhead+5), *(packhead+6), *(packhead+7));
			if (recvSumLen < num_received + netrecvOfs) {
				memcpy(context->m_receiveBuffer + context->m_receiveOfs,
					packhead, packlen + 4);
				context->m_receiveOfs += (packlen + 4);
				sem_post(&context->reciveOfsSem);

				packhead = packhead + packlen + 4;
				packlen = sNetToNative32(packhead);
				recvSumLen = recvSumLen + packlen + 4;
			} else if (recvSumLen > num_received + netrecvOfs) {
				/* Incomplete package */
				netrecvOfs = (packlen + 4) - (recvSumLen - num_received);
				if (packhead != netRecvBuffer)
					memmove(netRecvBuffer, packhead, netrecvOfs);
				break;
			} else {
				memcpy(context->m_receiveBuffer + context->m_receiveOfs,
					packhead, packlen + 4);
				context->m_receiveOfs += (packlen + 4);
				sem_post(&context->reciveOfsSem);
				netrecvOfs = 0;
				break;
			}
		}
		pthread_mutex_unlock(&context->m_receiveMutex);
	}
}

/*
 * @brief Update a connected context
 */
static void sUpdateContext(uSynergyContext *context)
{
	int packlen = 0;
	int ret;
	pthread_t receiveThread;

	if (pthread_create(&receiveThread, NULL, sRecvData, (void *)context))
		goto pthread_err;

	if (sem_init(&context->reciveOfsSem, 0, 0))
		goto semaphore_err;

	/* Eat packets */
	while (context->m_connected) {
		sem_wait(&context->reciveOfsSem);
		packlen = sNetToNative32(context->m_receiveBuffer);
//		if (packlen+4 <= context->m_receiveOfs) {
		if (packlen > 0) {
//printf("%c%c%c%c offset:%d packlen:%d\n", context->m_receiveBuffer[4],
//context->m_receiveBuffer[5], context->m_receiveBuffer[6],
//context->m_receiveBuffer[7], context->m_receiveOfs,packlen);
//			LOGI("%c%c%c%c offset:%d packlen:%d\n", context->m_receiveBuffer[4],
//				context->m_receiveBuffer[5], context->m_receiveBuffer[6],
//				context->m_receiveBuffer[7], context->m_receiveOfs,packlen);

			/* Process message */
			sProcessMessage(context, context->m_receiveBuffer);

			pthread_mutex_lock(&context->m_receiveMutex);
			/* Move packet to front of buffer */
			packlen = sNetToNative32(context->m_receiveBuffer);
			memmove(context->m_receiveBuffer, context->m_receiveBuffer +
				packlen + 4, context->m_receiveOfs - packlen - 4);
			context->m_receiveOfs -= packlen+4;

			memset(context->m_receiveBuffer + context->m_receiveOfs, 0,
				USYNERGY_RECEIVE_BUFFER_SIZE - context->m_receiveOfs);
			pthread_mutex_unlock(&context->m_receiveMutex);
		}
//		else if (context->m_hasReceivedHello) {
			/* Check for timeouts */
//			uint32_t cur_time = context->m_getTimeFunc();
			/* Timeout after 2 secs of inactivity (we received no CALV) */
//			if ((cur_time - context->m_lastMessageTime) > USYNERGY_IDLE_TIMEOUT)
//				sSetDisconnected(context);
//		}
	}
	pthread_join(receiveThread, NULL);
	return;

semaphore_err:
	sem_destroy(&context->reciveOfsSem);
pthread_err:
	perror("thread create error");
}

//-----------------------------------------------------------------------------
//	Public interface
//-----------------------------------------------------------------------------

/*
 * @brief Initialize uSynergy context
 */
void uSynergyInit(uSynergyContext *context, char *ClientName,
	int width, int height)
{
	char *tempStr;
	/* Zero memory */
	CookieType *cookie;
	cookie = malloc(sizeof(CookieType));
	memset(cookie, 0, sizeof(CookieType));
	context->m_cookie = cookie;

	/* Initialize to default state */
	tempStr = malloc(strlen(ClientName) + 1);
	context->m_clientName = strcpy(tempStr, ClientName);

	context->m_clientWidth	= width;
	context->m_clientHeight	= height;

	sSetDisconnected(context);
	build_key_translation_table();
}

/*
 * @brief Update uSynergy
 */
void uSynergyUpdate(uSynergyContext *context)
{
	/* Try to connect */
	if (context->m_connectFunc(context->m_cookie) &&
		context->m_connectDevice(context->m_cookie)) {
			context->m_connected = USYNERGY_TRUE;
	}

	if (context->m_connected) {
		/* Update context, receive data, call callbacks */
		pthread_mutexattr_t attr;
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&context->m_receiveMutex, &attr);
		sUpdateContext(context);
		pthread_mutex_destroy(&context->m_receiveMutex);
	}
}

/*
 * @brief Send clipboard data
 */
void uSynergySendClipboard(uSynergyContext *context, const char *text)
{
	// Calculate maximum size that will fit in a reply packet
	uint32_t overhead_size =	4 +	/* Message size */
								4 +	/* Message ID */
								1 +	/* Clipboard index */
								4 +	/* Sequence number */
								4 +	/* Rest of message size (because it's a
									   Synergy string from here on) */
								4 +	/* Number of clipboard formats */
								4 +	/* Clipboard format */
								4;	/* Clipboard data length */
	uint32_t max_length = USYNERGY_REPLY_BUFFER_SIZE - overhead_size;

	// Clip text to max length
	uint32_t text_length = (uint32_t)strlen(text);
	if (text_length > max_length) {
		char buffer[128];
		sprintf(buffer, "Clipboard buffer too small, clipboard truncated at %d \
			characters", max_length);
		sTrace(context, buffer);
		text_length = max_length;
	}

	// Assemble packet
	sAddString(context, "DCLP");
	/* Clipboard index */
	sAddUInt8(context, 0);
	sAddUInt32(context, context->m_sequenceNumber);
	/* Rest of message size: numFormats, format, length, data */
	sAddUInt32(context, 4+4+4+text_length);
	/* Number of formats (only text for now) */
	sAddUInt32(context, 1);
	sAddUInt32(context, USYNERGY_CLIPBOARD_FORMAT_TEXT);
	sAddUInt32(context, text_length);
	sAddString(context, text);
	sSendReply(context);
}

int uSynergyStart(uSynergyContext *context)
{
	context->m_updateServerAddr(context->m_cookie);
	uSynergyUpdate(context);
}

void uSynergyStop(uSynergyContext *context)
{
	sSetDisconnected(context);
}

void uSynergCleanUP(uSynergyContext *context)
{
	free((void *)context->m_clientName);
	free((void *)context->m_cookie);
}
