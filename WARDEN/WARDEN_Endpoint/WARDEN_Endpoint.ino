/*
 Name:		WARDEN_Endpoint.ino
 Created:	01-Dec-17 10:49:30 AM
 Author:	Dan Iorga
*/

#include "Persistence.h"
#include "endpoint.h"

Endpoint *endpoint;

// the setup function runs once when you press reset or power the board
void setup() {
	Memory::load_system_variables();
	endpoint = new Endpoint();
}

// the loop function runs over and over again until power down or reset
void loop() {
	endpoint->action();
}
