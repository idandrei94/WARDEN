/*
 Name:		WARDEN_Slave.ino
 Created:	18-Oct-17 11:35:04 AM
 Author:	Dan Iorga
*/

#include "slave.h"
#include "HC12_Driver.h"
#include "Persistence.h"

Slave *slave;

// the setup function runs once when you press reset or power the board
void setup() {
	Memory::load_system_variables();
	slave = new Slave();
	if (Variables::DEBUG)
	{
		Serial.println("#############");
		Serial.println("Slave started");
		Serial.println("#############");
	}
}

// the loop function runs over and over again until power down or reset
void loop() {
	slave->action();
}
