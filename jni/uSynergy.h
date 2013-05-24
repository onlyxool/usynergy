/*
 * uSynergy client -- Interface for the embedded Synergy client library
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

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>

#include "uinput.h"

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
//	Configuration
//-----------------------------------------------------------------------------

/*
 * @brief Determine endianness
 */
#if defined(USYNERGY_LITTLE_ENDIAN) && defined(USYNERGY_BIG_ENDIAN)
	/* Ambiguous: both endians specified */
	#error "Can't define both USYNERGY_LITTLE_ENDIAN and USYNERGY_BIG_ENDIAN"
#elif !defined(USYNERGY_LITTLE_ENDIAN) && !defined(USYNERGY_BIG_ENDIAN)
	/* Attempt to auto detect */
	#if defined(__LITTLE_ENDIAN__) || defined(LITTLE_ENDIAN) || \
		(_BYTE_ORDER == _LITTLE_ENDIAN)
		#define USYNERGY_LITTLE_ENDIAN
	#elif defined(__BIG_ENDIAN__) || defined(BIG_ENDIAN) || \
		(_BYTE_ORDER == _BIG_ENDIAN)
		#define USYNERGY_BIG_ENDIAN
	#else
		#error "Can't detect endian-nes, please defined either \
			USYNERGY_LITTLE_ENDIAN or USYNERGY_BIG_ENDIAN";
	#endif
#else
	/* User-specified endian-nes, nothing to do for us */
#endif

//-----------------------------------------------------------------------------
//	Types and Constants
//-----------------------------------------------------------------------------

/*
 * @brief Boolean type
 */
typedef int uSynergyBool;
#define USYNERGY_FALSE	0	/* False value */
#define USYNERGY_TRUE	1	/* True value */

/*
 * @brief User context type
 * The uSynergyCookie type is an opaque type that is used by uSynergy to
 *	communicate to the client. It is passed along to callback
 *  functions as context.
 */
typedef struct {
	int sockfd;
	struct sockaddr_in server_addr;
	char *device_name;
	struct input_id device_id;
	int uinput_keyboard;
	int uinput_mouse;
	int uinput_joystick;
} CookieType, *uSynergyCookie;

/*
 * @brief Clipboard types
 */
enum uSynergyClipboardFormat {
	/* Text format, UTF-8, newline is LF */
	USYNERGY_CLIPBOARD_FORMAT_TEXT	= 0,

	/* Bitmap format, BMP 24/32bpp, BI_RGB */
	USYNERGY_CLIPBOARD_FORMAT_BITMAP= 1,

	/* HTML format, HTML fragment, UTF-8, newline is LF */
	USYNERGY_CLIPBOARD_FORMAT_HTML	= 2,
};

/*
 * @brief Constants and limits
 */
/* Maximum number of supported joysticks */
#define USYNERGY_NUM_JOYSTICKS			4

/* Major protocol version */
#define USYNERGY_PROTOCOL_MAJOR			1
/* Minor protocol version */
#define USYNERGY_PROTOCOL_MINOR			4

/* Timeout in milliseconds before reconnecting */
#define USYNERGY_IDLE_TIMEOUT			5000

/* Maximum length of traced message */
#define USYNERGY_TRACE_BUFFER_SIZE		1024
/* Maximum size of a reply packet */
#define USYNERGY_REPLY_BUFFER_SIZE		1024
/* Maximum size of an incoming packet */
#define USYNERGY_RECEIVE_BUFFER_SIZE	4096
/* Maximum size of a receive packet*/
#define USYNERGY_NETRECV_BUFFER_SIZE	1024


/*
 * @brief Keyboard constants
 */
#define USYNERGY_MODIFIER_SHIFT			0x0001	/* Shift key modifier */
#define USYNERGY_MODIFIER_CTRL			0x0002	/* Ctrl key modifier */
#define USYNERGY_MODIFIER_ALT			0x0004	/* Alt key modifier */
#define USYNERGY_MODIFIER_META			0x0008	/* Meta key modifier */
#define USYNERGY_MODIFIER_WIN			0x0010	/* Windows key modifier */
#define USYNERGY_MODIFIER_ALT_GR		0x0020	/* AltGr key modifier */
#define USYNERGY_MODIFIER_LEVEL5LOCK	0x0040	/* Level5Lock key modifier */
#define USYNERGY_MODIFIER_CAPSLOCK		0x1000	/* CapsLock key modifier */
#define USYNERGY_MODIFIER_NUMLOCK		0x2000	/* NumLock key modifier */
#define USYNERGY_MODIFIER_SCROLLOCK		0x4000	/* ScrollLock key modifier */

//-----------------------------------------------------------------------------
//	Context
//-----------------------------------------------------------------------------

/*
 * @brief uSynergy context
 */
typedef struct {
	/* Mandatory configuration data, filled in by client */

	/* Connect function */
	uSynergyBool (*m_connectFunc)(uSynergyCookie cookie);

	void (*m_updateServerAddr)(uSynergyCookie cookie, char *addr, int port);

	/* Send data function */
	uSynergyBool (*m_sendFunc)(uSynergyCookie cookie, const uint8_t *buffer,
		int length);

	/* Receive data function */
	uSynergyBool (*m_receiveFunc)(uSynergyCookie cookie, uint8_t *buffer,
		int maxLength, int* outLength);

	/* connetct input device */
	uSynergyBool (*m_connectDevice)(uSynergyCookie cookie);

	void (*m_disconnectDevice)(uSynergyCookie cookie);

	/* Thread sleep function */
	void (*m_sleepFunc)(uSynergyCookie cookie, int timeMs);

	/* Get current time function */
	uint32_t (*m_getTimeFunc)();

	/* Name of Synergy Screen / Client */
	const char* m_clientName;

	/* Width of screen */
	uint16_t m_clientWidth;

	/* Height of screen */
	uint16_t m_clientHeight;

	/* Optional configuration data, filled in by client */
	/* Cookie pointer passed to callback functions (can be NULL) */
	uSynergyCookie m_cookie;

	/* Function for tracing status (can be NULL) */
	void (*m_traceFunc)(uSynergyCookie cookie, const char *text);

	/* Callback for entering and leaving screen */
	void (*m_screenActiveCallback)(uSynergyCookie cookie, uSynergyBool active);

	/* Callback for mouse events */
	uSynergyBool (*m_mouseMoveCallback)(uSynergyCookie cookie,
		int32_t x, int32_t y);

	uSynergyBool (*m_mouseUpCallback)(uSynergyCookie cookie,
		uSynergyBool buttonLeft, uSynergyBool buttonRight,
		uSynergyBool buttonMiddle);

	uSynergyBool (*m_mouseDownCallback)(uSynergyCookie cookie,
		uSynergyBool buttonLeft, uSynergyBool buttonRight,
		uSynergyBool buttonMiddle);

	uSynergyBool (*m_mouseWheelCallback)(uSynergyCookie cookie, int16_t wheelX,
		int16_t wheelY);

	/* Callback for keyboard events */
	void (*m_keyboardCallback)(uSynergyCookie cookie, uint16_t key,
		uint16_t modifiers, uSynergyBool down, uSynergyBool repeat);

	/* Callback for joystick events */
	void (*m_joystickCallback)(uSynergyCookie cookie, uint8_t joyNum,
		uint16_t buttons, int8_t leftStickX, int8_t leftStickY,
		int8_t rightStickX, int8_t rightStickY);

	/* Callback for clipboard events */
	void (*m_clipboardCallback)(uSynergyCookie cookie,
		enum uSynergyClipboardFormat format, const uint8_t *data,
		uint32_t size);

	/* State data, used internall by client, initialized by uSynergyInit() */
	/* Is our socket connected? */
	uSynergyBool m_connected;

	/* Have we received a 'Hello' from the server? */
	uSynergyBool m_hasReceivedHello;

	uSynergyBool m_ongoing;

	/* Is Synergy active (i.e. this client is receiving input messages?) */
	uSynergyBool m_isCaptured;

	/* Time at which last message was received */
	uint32_t m_lastMessageTime;

	/* Packet sequence number */
	uint32_t m_sequenceNumber;

	/* Receive buffer */
	uint8_t m_receiveBuffer[USYNERGY_RECEIVE_BUFFER_SIZE];

	/* Receive buffer offset */
	int	m_receiveOfs;

	pthread_mutex_t m_receiveMutex;

	/* semaphore of package number in receive Buffer*/
	sem_t reciveOfsSem;

	/* Reply buffer */
	uint8_t	m_replyBuffer[USYNERGY_REPLY_BUFFER_SIZE];

	/* Write offset into reply buffer */
	uint8_t* m_replyCur;

	uint16_t m_mouseX_old;
	uint16_t m_mouseY_old;

	/* Mouse X position */
	uint16_t m_mouseX;

	/* Mouse Y position */
	uint16_t m_mouseY;

	/* Mouse wheel X position */
	int16_t	m_mouseWheelX;

	/* Mouse wheel Y position */
	int16_t	m_mouseWheelY;

	/* Mouse left button */
	uSynergyBool m_mouseButtonLeft;

	/* Mouse right button */
	uSynergyBool m_mouseButtonRight;

	/* Mouse middle button */
	uSynergyBool m_mouseButtonMiddle;

	/* Joystick stick position in 2 axes for 2 sticks */
	int8_t m_joystickSticks[USYNERGY_NUM_JOYSTICKS][4];

	/* Joystick button state */
	uint16_t m_joystickButtons[USYNERGY_NUM_JOYSTICKS];
} uSynergyContext;

//-----------------------------------------------------------------------------
//	Interface
//-----------------------------------------------------------------------------

/*
 * @brief Initialize uSynergy context

 * This function initializes @a context for use. Call this function directly
 * after creating the context, before filling in any configuration data in it.
 * Not calling this function will cause undefined behavior.
 * @param context Context to be initialized
 */
extern void uSynergyInit(uSynergyContext *context, char *ClientName, int width, int height);

/*
 * @brief Update uSynergy

 * This function updates uSynergy and does the bulk of the work. It does
 * connection management, receiving data, reconnecting after errors or timeouts
 * and so on. It assumes that networking operations are blocking and it can
 * suspend the current thread if it needs to wait. It is best practice to call
 * uSynergyUpdate from a background thread so it is responsive.

 * Because uSynergy relies mostly on blocking calls it will mostly stay in
 * thread sleep state waiting for system mutexes and won't eat much memory.

 * uSynergyUpdate doesn't do any memory allocations or have any side effects
 * beyond those of the callbacks it calls.

 * @param context	Context to be updated
 */
extern void uSynergyUpdate(uSynergyContext *context);

/*
 * @brief Send clipboard data

 * This function sets new clipboard data and sends it to the server. Use this
 * function if your client cuts or copies data onto the clipboard that it needs
 * to share with the server.

 * Currently there is only support for plaintext, but HTML and image data could
 * be supported with some effort.

 * @param context	Context to send clipboard data to
 * @param text		Text to set to the clipboard
 */
extern void uSynergySendClipboard(uSynergyContext *context, const char *text);

extern void uSynergyStart(uSynergyContext *context, char *addr, int port);

extern void uSynergyStop(uSynergyContext *context);

extern void uSynergCleanUP(uSynergyContext *context);
#ifdef __cplusplus
};
#endif
