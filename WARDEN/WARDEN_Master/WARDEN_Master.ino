/*
 Name:		WARDEN_Master.ino
 Created:	17-Oct-17 2:24:43 PM
 Author:	Dan Iorga
*/

#include "master.h"
#include "Persistence.h"

Master *master;

// the setup function runs once when you press reset or power the board
void setup() {
	Memory::load_system_variables();
	master = new Master();
	if (Variables::DEBUG)
	{
		Serial.println("##############");
		Serial.println("Master started");
		Serial.println("##############");
	}
}

// the loop function runs over and over again until power down or reset
void loop() {
	master->action();
}
