#pragma once
#include "message_handler.h"
#include "HC12_Driver.h"
#include "message.h"
#include "Serial_Driver.h"

class Endpoint
{
private:
	HC12_Driver driver;
	Serial_Driver s_driver;
	uint8_t current_neighbour;
	CallNeighboursMessage call_msg;
	uint8_t send_buffer[Message::MESSAGE_LENGTH];
	uint8_t receive_buffer[Message::MESSAGE_LENGTH];
	Message_Handler handler;
public:
	Endpoint();
	void action();
	void forwardToNeighbours(uint8_t current_neighbour);
};