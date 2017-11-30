/*
 Name:		WARDEN.ino
 Created:	15-Sep-17 9:24:35 AM
 Author:	Dan Iorga
*/

#include "HC12_Driver.h"
#include "list.h"
#include "Serial_Driver.h"
#include "Message.h"

Message* messages[3];
uint8_t i = 0;
uint8_t* buffer;
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	buffer = new uint8_t[Message::MESSAGE_LENGTH];
	uint8_t data[48];
	for (uint8_t i = 0; i < 48; ++i) {
		data[i] = i;
	}
	/**	send_to			0x0F
	*	received_from	0x09
	*	msg_source		0x0A
	*/
	messages[0] = new ConfirmPingMessage(0x0f, 0x0a, 0x09, 0xff);
	/**	send_to			0x0F
	*	received_from	0x09
	*	msg_source		0x0A
	*/
	messages[1] = new AccidentMessage(0x0f, 0x0a, 0x09, 0xff, data);
	/**	send_to			0x0F
	*	received_from	0x09
	*	msg_source		0x0A
	*/
	messages[2] = new CancelAccidentMessage(0x0f, 0x0a, 0x09, 0xff, data);
}

// the loop function runs over and over again until power down or reset
void loop() {
	messages[0]->gen_message_string(buffer);
	i %= 3;
	Serial.write(buffer, Message::MESSAGE_LENGTH);
}
