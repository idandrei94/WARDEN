#pragma once
#include "message_handler.h"
#include "HC12_Driver.h"
#include "message.h"

class Slave
{
private:
	Message_Handler *handler;
	HC12_Driver *group_driver;
	HC12_Driver *bridge_driver;
	CallNeighboursMessage call_msg;
	uint8_t group_send_buffer[Message::MESSAGE_LENGTH];
	uint8_t group_receive_buffer[Message::MESSAGE_LENGTH];
	uint8_t bridge_send_buffer[Message::MESSAGE_LENGTH];
	uint8_t bridge_receive_buffer[Message::MESSAGE_LENGTH];
	bool group_bridge_switch;
public:
	Slave();
	void action();
};