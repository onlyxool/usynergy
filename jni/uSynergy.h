/*
uSynergy client -- Interface for the embedded Synergy client library
  version 1.0.0, July 7th, 2012

Copyright (c) 2012 Alex Evans

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "uinput.h"

#ifdef __cplusplus
extern "C" {
#endif



//---------------------------------------------------------------------------------------------------------------------
//	Configuration
//---------------------------------------------------------------------------------------------------------------------



/**
@brief Determine endianness
**/
#if defined(USYNERGY_LITTLE_ENDIAN) && defined(USYNERGY_BIG_ENDIAN)
	/* Ambiguous: both endians specified */
	#error "Can't define both USYNERGY_LITTLE_ENDIAN and USYNERGY_BIG_ENDIAN"
#elif !defined(USYNERGY_LITTLE_ENDIAN) && !defined(USYNERGY_BIG_ENDIAN)
	/* Attempt to auto detect */
	#if defined(__LITTLE_ENDIAN__) || defined(LITTLE_ENDIAN) || (_BYTE_ORDER == _LITTLE_ENDIAN)
		#define USYNERGY_LITTLE_ENDIAN
	#elif defined(__BIG_ENDIAN__) || defined(BIG_ENDIAN) || (_BYTE_ORDER == _BIG_ENDIAN)
		#define USYNERGY_BIG_ENDIAN
	#else
		#error "Can't detect endian-nes, please defined either USYNERGY_LITTLE_ENDIAN or USYNERGY_BIG_ENDIAN";
	#endif
#else
	/* User-specified endian-nes, nothing to do for us */
#endif



//---------------------------------------------------------------------------------------------------------------------
//	Types and Constants
//---------------------------------------------------------------------------------------------------------------------



/**
@brief Boolean type
**/
typedef int			uSynergyBool;
#define				USYNERGY_FALSE					0				/* False value */
#define				USYNERGY_TRUE					1				/* True value */

/**
@brief User context type

The uSynergyCookie type is an opaque type that is used by uSynergy to communicate to the client. It is passed along to
callback functions as context.
**/
typedef struct {
	int sockfd;
	char *server_name;
	struct sockaddr_in server_addr;
	char* device_name;
	struct input_id device_id;
	int uinput_keyboard;
	int uinput_mouse;
	int uinput_joystick;
} CookieType, *uSynergyCookie;



/**
@brief Clipboard types
**/
enum uSynergyClipboardFormat
{
	USYNERGY_CLIPBOARD_FORMAT_TEXT					= 0,			/* Text format, UTF-8, newline is LF */
	USYNERGY_CLIPBOARD_FORMAT_BITMAP				= 1,			/* Bitmap format, BMP 24/32bpp, BI_RGB */
	USYNERGY_CLIPBOARD_FORMAT_HTML					= 2,			/* HTML format, HTML fragment, UTF-8, newline is LF */
};



/**
@brief Constants and limits
**/
#define				USYNERGY_NUM_JOYSTICKS			4				/* Maximum number of supported joysticks */

#define				USYNERGY_PROTOCOL_MAJOR			1				/* Major protocol version */
#define				USYNERGY_PROTOCOL_MINOR			4				/* Minor protocol version */

#define				USYNERGY_IDLE_TIMEOUT			5000			/* Timeout in milliseconds before reconnecting */

#define				USYNERGY_TRACE_BUFFER_SIZE		1024			/* Maximum length of traced message */
#define				USYNERGY_REPLY_BUFFER_SIZE		1024			/* Maximum size of a reply packet */
#define				USYNERGY_RECEIVE_BUFFER_SIZE	4096			/* Maximum size of an incoming packet */



/**
@brief Keyboard constants
**/
#define				USYNERGY_MODIFIER_SHIFT			0x0001			/* Shift key modifier */
#define				USYNERGY_MODIFIER_CTRL			0x0002			/* Ctrl key modifier */
#define				USYNERGY_MODIFIER_ALT			0x0004			/* Alt key modifier */
#define				USYNERGY_MODIFIER_META			0x0008			/* Meta key modifier */
#define				USYNERGY_MODIFIER_WIN			0x0010			/* Windows key modifier */
#define				USYNERGY_MODIFIER_ALT_GR		0x0020			/* AltGr key modifier */
#define				USYNERGY_MODIFIER_LEVEL5LOCK	0x0040			/* Level5Lock key modifier */
#define				USYNERGY_MODIFIER_CAPSLOCK		0x1000			/* CapsLock key modifier */
#define				USYNERGY_MODIFIER_NUMLOCK		0x2000			/* NumLock key modifier */
#define				USYNERGY_MODIFIER_SCROLLOCK		0x4000			/* ScrollLock key modifier */


//---------------------------------------------------------------------------------------------------------------------
//	Context
//---------------------------------------------------------------------------------------------------------------------



/**
@brief uSynergy context
**/
typedef struct //uSynergyContext
{
	/* Mandatory configuration data, filled in by client */
	uSynergyBool (*m_connectFunc)(uSynergyCookie cookie);	/* Connect function */
	uSynergyBool (*m_sendFunc)(uSynergyCookie cookie,
		const uint8_t *buffer, int length);					/* Send data function */
	uSynergyBool (*m_receiveFunc)(uSynergyCookie cookie,
		uint8_t *buffer, int maxLength, int* outLength);	/* Receive data function */
	uSynergyBool (*m_connectDevice)(uSynergyCookie cookie);	/* connetct input device */
	void (*m_disconnectDevice)(uSynergyCookie cookie);
	void (*m_sleepFunc)(uSynergyCookie cookie, int timeMs);	/* Thread sleep function */
	uint32_t    (*m_getTimeFunc)();							/* Get current time function */
	const char*						m_clientName;									/* Name of Synergy Screen / Client */
	uint16_t						m_clientWidth;									/* Width of screen */
	uint16_t						m_clientHeight;									/* Height of screen */

	/* Optional configuration data, filled in by client */
	uSynergyCookie					m_cookie;										/* Cookie pointer passed to callback functions (can be NULL) */
	void (*m_traceFunc)(uSynergyCookie cookie, const char *text); /* Function for tracing status (can be NULL) */
	void (*m_screenActiveCallback)(uSynergyCookie cookie,
		uSynergyBool active);								/* Callback for entering and leaving screen */
	uSynergyBool (*m_mouseMoveCallback)(uSynergyCookie cookie, int32_t x,
		int32_t y);/* Callback for mouse events */
	uSynergyBool (*m_mouseUpCallback)(uSynergyCookie cookie, uSynergyBool buttonLeft,
		uSynergyBool buttonRight, uSynergyBool buttonMiddle);
	uSynergyBool (*m_mouseDownCallback)(uSynergyCookie cookie, uSynergyBool buttonLeft,
		uSynergyBool buttonRight, uSynergyBool buttonMiddle);
	uSynergyBool (*m_mouseWheelCallback)(uSynergyCookie cookie, int16_t wheelX, int16_t wheelY);
	void (*m_keyboardCallback)(uSynergyCookie cookie, uint16_t key,
		uint16_t modifiers, uSynergyBool down, uSynergyBool repeat); /* Callback for keyboard events */
	void (*m_joystickCallback)(uSynergyCookie cookie, uint8_t joyNum, /* Callback for joystick events */
		uint16_t buttons, int8_t leftStickX, int8_t     leftStickY,
		int8_t rightStickX, int8_t rightStickY);
	void (*m_clipboardCallback)(uSynergyCookie cookie,               /* Callback for clipboard events */
		enum uSynergyClipboardFormat format, const uint8_t *data,
		uint32_t size);

	/* State data, used internall by client, initialized by uSynergyInit() */
	uSynergyBool					m_connected;									/* Is our socket connected? */
	uSynergyBool					m_hasReceivedHello;								/* Have we received a 'Hello' from the server? */
	uSynergyBool					m_isCaptured;									/* Is Synergy active (i.e. this client is receiving input messages?) */
	uint32_t						m_lastMessageTime;								/* Time at which last message was received */
	uint32_t						m_sequenceNumber;								/* Packet sequence number */
	uint8_t							m_receiveBuffer[USYNERGY_RECEIVE_BUFFER_SIZE];	/* Receive buffer */
	int								m_receiveOfs;									/* Receive buffer offset */
	pthread_mutex_t					m_receiveMutex;
	uint8_t							m_replyBuffer[USYNERGY_REPLY_BUFFER_SIZE];		/* Reply buffer */
	uint8_t*						m_replyCur;										/* Write offset into reply buffer */
	uint16_t						m_mouseX_old;
	uint16_t						m_mouseY_old;
	uint16_t						m_mouseX;										/* Mouse X position */
	uint16_t						m_mouseY;										/* Mouse Y position */
	int16_t							m_mouseWheelX;									/* Mouse wheel X position */
	int16_t							m_mouseWheelY;									/* Mouse wheel Y position */
	uSynergyBool					m_mouseButtonLeft;								/* Mouse left button */
	uSynergyBool					m_mouseButtonRight;								/* Mouse right button */
	uSynergyBool					m_mouseButtonMiddle;							/* Mouse middle button */
	int8_t							m_joystickSticks[USYNERGY_NUM_JOYSTICKS][4];	/* Joystick stick position in 2 axes for 2 sticks */
	uint16_t						m_joystickButtons[USYNERGY_NUM_JOYSTICKS];		/* Joystick button state */
} uSynergyContext;



//---------------------------------------------------------------------------------------------------------------------
//	Interface
//---------------------------------------------------------------------------------------------------------------------



/**
@brief Initialize uSynergy context

This function initializes @a context for use. Call this function directly after
creating the context, before filling in any configuration data in it. Not calling
this function will cause undefined behavior.

@param context	Context to be initialized
**/
extern void		uSynergyInit(uSynergyContext *context);



/**
@brief Update uSynergy

This function updates uSynergy and does the bulk of the work. It does connection management,
receiving data, reconnecting after errors or timeouts and so on. It assumes that networking
operations are blocking and it can suspend the current thread if it needs to wait. It is
best practice to call uSynergyUpdate from a background thread so it is responsive.

Because uSynergy relies mostly on blocking calls it will mostly stay in thread sleep state
waiting for system mutexes and won't eat much memory.

uSynergyUpdate doesn't do any memory allocations or have any side effects beyond those of
the callbacks it calls.

@param context	Context to be updated
**/
extern void		uSynergyUpdate(uSynergyContext *context);



/**
@brief Send clipboard data

This function sets new clipboard data and sends it to the server. Use this function if
your client cuts or copies data onto the clipboard that it needs to share with the
server.

Currently there is only support for plaintext, but HTML and image data could be
supported with some effort.

@param context	Context to send clipboard data to
@param text		Text to set to the clipboard
**/
extern void		uSynergySendClipboard(uSynergyContext *context, const char *text);



#ifdef __cplusplus
};
#endif
