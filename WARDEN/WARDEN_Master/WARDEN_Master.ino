/*
 Name:		WARDEN_Master.ino
 Created:	17-Oct-17 2:24:43 PM
 Author:	Dan Iorga
*/

#include "master.h"

Master *master;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	master = new Master();
	Serial.println("##############");
	Serial.println("Master started");
	Serial.println("##############");
}

// the loop function runs over and over again until power down or reset
void loop() {
	master->action();
}
