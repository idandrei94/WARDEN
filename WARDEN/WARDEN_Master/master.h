#pragma once
#include "message_handler.h"
#include "HC12_Driver.h"
#include "message.h"

class Master
{
private:
	HC12_Driver driver;
	uint8_t current_neighbour;
	CallNeighboursMessage call_msg;
	uint8_t send_buffer[Message::MESSAGE_LENGTH];
	uint8_t receive_buffer[Message::MESSAGE_LENGTH];
	Message_Handler handler;
	void forwardToNeighbours();
public:
	Master();
	void action();
};