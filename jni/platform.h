#include "uSynergy.h"

//---------------------------------------------------------------------------------------------------------------------
//	Functions and Callbacks
//---------------------------------------------------------------------------------------------------------------------

/**
@brief Connect function

This function is called when uSynergy needs to connect to the host. It doesn't imply a network implementation or
destination address, that must all be handled on the user side. The function should return USYNERGY_TRUE if a
connection was established or USYNERGY_FALSE if it could not connect.

When network errors occur (e.g. uSynergySend or uSynergyReceive fail) then the connect call will be called again
so the implementation of the function must close any old connections and clean up resources before retrying.

@param cookie		Cookie supplied in the Synergy context
**/
static uSynergyBool uSynergyConnectFunc(uSynergyCookie cookie);

/**
@brief Send function

This function is called when uSynergy needs to send something over the default connection. It should return
USYNERGY_TRUE if sending succeeded and USYNERGY_FALSE otherwise. This function should block until the send
operation is completed.

@param cookie		Cookie supplied in the Synergy context
@param buffer		Address of buffer to send
@param length		Length of buffer to send
**/
static uSynergyBool uSynergySendFunc(uSynergyCookie cookie, const uint8_t *buffer, int length);

/**
@brief Receive function

This function is called when uSynergy needs to receive data from the default connection. It should return
USYNERGY_TRUE if receiving data succeeded and USYNERGY_FALSE otherwise. This function should block until data
has been received and wait for data to become available. If @a outLength is set to 0 upon completion it is
assumed that the connection is alive, but still in a connecting state and needs time to settle.

@param cookie		Cookie supplied in the Synergy context
@param buffer		Address of buffer to receive data into
@param maxLength	Maximum amount of bytes to write into the receive buffer
@param outLength	Address of integer that receives the actual amount of bytes written into @a buffer
**/
static uSynergyBool uSynergyReceiveFunc(uSynergyCookie cookie, uint8_t *buffer, int maxLength, int* outLength);

/**
@brief Thread sleep function

This function is called when uSynergy wants to suspend operation for a while before retrying an operation. It
is mostly used when a socket times out or disconnect occurs to prevent uSynergy from continuously hammering a
network connection in case the network is down.

@param cookie		Cookie supplied in the Synergy context
@param timeMs		Time to sleep the current thread (in milliseconds)
**/
static void uSynergySleepFunc(uSynergyCookie cookie, int timeMs);

/**
@brief Get time function

This function is called when uSynergy needs to know the current time. This is used to determine when timeouts
have occured. The time base should be a cyclic millisecond time value.

@returns			Time value in milliseconds
**/
static uint32_t	uSynergyGetTimeFunc();

static uSynergyBool uSynergyConnectDevice(uSynergyCookie cookie);
static void uSynergyDisconnectDevice(uSynergyCookie cookie);

/**
@brief Trace function

This function is called when uSynergy wants to trace something. It is optional to show these messages, but they
are often useful when debugging. uSynergy onli traces major events like connecting and disconnecting. Usually
only a single trace is shown when the connection is established and no more trace are called.

@param cookie		Cookie supplied in the Synergy context
@param text			Text to be traced
**/
static void uSynergyTraceFunc(uSynergyCookie cookie, const char *text);

/**
@brief Screen active callback

This callback is called when Synergy makes the screen active or inactive. This
callback is usually sent when the mouse enters or leaves the screen.

@param cookie		Cookie supplied in the Synergy context
@param active		Activation flag, 1 if the screen has become active, 0 if the screen has become inactive
**/
static void uSynergyScreenActiveCallback(uSynergyCookie cookie, uSynergyBool active);

/**
@brief Mouse callback

This callback is called when a mouse events happens. The mouse X and Y position,
wheel and button state is communicated in the message. It's up to the user to
interpret if this is a mouse up, down, double-click or other message.

@param cookie		Cookie supplied in the Synergy context
@param x			Mouse X position
@param y			Mouse Y position
@param wheelX		Mouse wheel X position
@param wheelY		Mouse wheel Y position
@param buttonLeft	Left button pressed status, 0 for released, 1 for pressed
@param buttonMiddle	Middle button pressed status, 0 for released, 1 for pressed
@param buttonRight	Right button pressed status, 0 for released, 1 for pressed
**/
static uSynergyBool uSynergyMouseMoveCallback(uSynergyCookie cookie, int32_t x, int32_t y);
static uSynergyBool uSynergyMouseUpCallback(uSynergyCookie cookie, uSynergyBool buttonLeft, uSynergyBool buttonRight, uSynergyBool buttonMiddle);
static uSynergyBool uSynergyMouseDownCallback(uSynergyCookie cookie, uSynergyBool buttonLeft, uSynergyBool buttonRight, uSynergyBool buttonMiddle);
static uSynergyBool uSynergyMouseWheelCallback(uSynergyCookie cookie, int16_t wheelX, int16_t wheelY);

/**
@brief Key event callback

This callback is called when a key is pressed or released.

@param cookie		Cookie supplied in the Synergy context
@param key			Key code of key that was pressed or released
@param modifiers	Status of modifier keys (alt, shift, etc.)
@param down			Down or up status, 1 is key is pressed down, 0 if key is released (up)
@param repeat		Repeat flag, 1 if the key is down because the key is repeating, 0 if the key is initially pressed by the user
**/
static void uSynergyKeyboardCallback(uSynergyCookie cookie, uint16_t key, uint16_t modifiers, uSynergyBool down, uSynergyBool repeat);

/**
@brief Joystick event callback

This callback is called when a joystick stick or button changes. It is possible that multiple callbacks are
fired when different sticks or buttons change as these are individual messages in the packet stream. Each
callback will contain all the valid state for the different axes and buttons. The last callback received will
represent the most current joystick state.

@param cookie		Cookie supplied in the Synergy context
@param joyNum		Joystick number, always in the range [0 ... USYNERGY_NUM_JOYSTICKS>
@param buttons		Button pressed mask
@param leftStickX	Left stick X position, in range [-127 ... 127]
@param leftStickY	Left stick Y position, in range [-127 ... 127]
@param rightStickX	Right stick X position, in range [-127 ... 127]
@param rightStickY	Right stick Y position, in range [-127 ... 127]
**/
static void uSynergyJoystickCallback(uSynergyCookie cookie, uint8_t joyNum, uint16_t buttons, int8_t leftStickX, int8_t leftStickY, int8_t rightStickX, int8_t rightStickY);

/**
@brief Clipboard event callback

This callback is called when something is placed on the clipboard. Multiple callbacks may be fired for
multiple clipboard formats if they are supported. The data provided is read-only and may not be modified
by the application.

@param cookie		Cookie supplied in the Synergy context
@param format		Clipboard format
@param data			Memory area containing the clipboard raw data
@param size			Size of clipboard data
**/
static void uSynergyClipboardCallback(uSynergyCookie cookie, enum uSynergyClipboardFormat format, const uint8_t *data, uint32_t size);
