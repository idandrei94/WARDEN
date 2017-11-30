#include "message_handler.h"

Message_Handler::Message_Handler(uint8_t _list_size) : bridge_list(_list_size), message_list(_list_size), slave_state(Message_Handler::State::FREE)
{
	uint8_t *arr = new uint8_t[64];
	RequestPingMessage m(Variables::BROADCAST_ID, Variables::END_ID, 0xA5, 20);
	m.gen_message_string(arr);
	bridge_list.push_back(arr);
}

Message_Handler::Action Message_Handler::receive_message(uint8_t buffer[], Message_Handler::Source source)
{

	if (!Message::verify_checksum(buffer)) {
		if(Variables::DEBUG)
			Serial.println("Message integrity check failed");
		return Message_Handler::Action::ERROR;
	}
	if (Variables::DEBUG)
	{
		Serial.print("Message received, source: ");
	}
	return source == Message_Handler::Source::GROUP ? receive_group_message(buffer) : receive_bridge_message(buffer);
}

Message_Handler::Action Message_Handler::receive_group_message(uint8_t buffer[])
{
	if(Variables::DEBUG)
		Serial.println("group");
	uint8_t *message = nullptr;
	switch (Message::convert_type(buffer[0]))
	{
	case Message::MessageType::CALL_NEIGHBOURS:
		Serial.println("  Type: CALL_NEIGBOURS");
		if (buffer[7] != Variables::NEIGHBOUR_ID) // Ignore the call if it's for another neighbour
		{
			if (Variables::DEBUG) {
				Serial.print("    Message for another neighbour: ");
				Serial.println(buffer[7]);
			}
			return Message_Handler::Action::NO_ACTION;
		}
		slave_state = Message_Handler::State::CONNECTED;
		if (buffer[8] == 1)// If the master sends a message
		{
			if(Variables::DEBUG)
				Serial.println("    Confirming master message");
			return Message_Handler::Action::CONFIRM;
		}
		else// Master wants a message
		{
			if (bridge_list.size() == 0) // If there's nothing to send
			{
				if(Variables::DEBUG)
					Serial.println("    No messages for the master");
				slave_state == Message_Handler::State::FREE;
				return Message_Handler::Action::NO_ACTION;
			}
			else
			{
				if (Variables::DEBUG)
					Serial.println("Sending message to master");
				return Message_Handler::Action::SEND_MESSAGE;
			}
		}
	case Message::MessageType::ERROR_MESSAGE:// We ignore messages that aren't meant for us
		return slave_state == Message_Handler::State::CONNECTED ? Message_Handler::Action::RESEND : Message_Handler::Action::NO_ACTION;
	case Message::MessageType::CONFIRM_MESSAGE:
		slave_state = Message_Handler::State::FREE;
		return Message_Handler::Action::NO_ACTION;
	case Message::MessageType::CALL_NEIGHBOURS_REPLY:
		slave_state = Message_Handler::State::FREE;
		return Message_Handler::Action::NO_ACTION;
	case Message::MessageType::CANCEL_ACCIDENT:
		if (Variables::DEBUG) {
			Serial.println("Adding CANCEL_ACCIDENT message to the Queue");
		}
		message = new uint8_t[Message::MESSAGE_LENGTH];
		for (uint8_t i = 0; i < Message::MESSAGE_LENGTH; ++i) {
			message[i] = buffer[i];
		}
		if (bridge_list.push_back(message) < 0)
			delete message;
		slave_state = Message_Handler::State::FREE;
		return Message_Handler::Action::CONFIRM;
	case Message::MessageType::ACCIDENT:
		if (Variables::DEBUG) {
			Serial.println("Adding ACCIDENT message to the Queue");
		}
		message = new uint8_t[Message::MESSAGE_LENGTH];
		for (uint8_t i = 0; i < Message::MESSAGE_LENGTH; ++i) {
			message[i] = buffer[i];
		}
		if (bridge_list.push_back(message) < 0)
			delete message;
		slave_state = Message_Handler::State::FREE;
		return Message_Handler::Action::CONFIRM;
	case Message::MessageType::REQ_PING:
		if (slave_state != State::CONNECTED)
		{
			return Message_Handler::Action::NO_ACTION;
		}
		if (Variables::DEBUG) {
				Serial.println("Adding REQ_PING message to the Queue");
		}
		message = new uint8_t[Message::MESSAGE_LENGTH];
		for (uint8_t i = 0; i < Message::MESSAGE_LENGTH; ++i) {
			message[i] = buffer[i];
		}
		if (bridge_list.push_back(message) < 0)
			delete message;
		if (buffer[1] << 8 + buffer[2] == Variables::ID || buffer[1] << 8 + buffer[2] == Variables::BROADCAST_ID)
		{
			message = new uint8_t[Message::MESSAGE_LENGTH];
			ConfirmPingMessage m(Variables::END_ID, Variables::ID, Variables::ID, Variables::TTL);
			m.gen_message_string(message);
			if (bridge_list.push_back(message) < 0)
				delete message;
			if (buffer[1] << 8 + buffer[2] == Variables::BROADCAST_ID)
			{
				message = new uint8_t[Message::MESSAGE_LENGTH];
				for (uint8_t i = 0; i < Message::MESSAGE_LENGTH; ++i) {
					message[i] = buffer[i];
				}
				if (bridge_list.push_back(message) < 0)
					delete message;
			}
		}
		slave_state = Message_Handler::State::FREE;
		return Message_Handler::Action::CONFIRM;
	case Message::MessageType::CONFIRM_PING:
		if (slave_state != State::CONNECTED)
		{
			return Message_Handler::Action::NO_ACTION;
		}
		if (Variables::DEBUG) {
			Serial.println("Adding CONFIRM_PING message to the Queue");
		}
		message = new uint8_t[Message::MESSAGE_LENGTH];
		for (uint8_t i = 0; i < Message::MESSAGE_LENGTH; ++i) {
			message[i] = buffer[i];
		}
		if (bridge_list.push_back(message) < 0)
			delete message;
		slave_state = Message_Handler::State::FREE;
		return Message_Handler::Action::CONFIRM;
	default:
		if (Variables::DEBUG)
		{
			Serial.print("Invalid message type: ");
			Serial.println(buffer[0]);
		}
		return Message_Handler::Action::NO_ACTION;
	}
}

Message_Handler::Action Message_Handler::receive_bridge_message(uint8_t buffer[])
{
	if (Variables::DEBUG)
		Serial.println("bridge");
	uint8_t *message;
	switch (Message::convert_type(buffer[0]))
	{
	case Message::MessageType::CALL_NEIGHBOURS:
		return Message_Handler::Action::REPLY;
	case Message::MessageType::CALL_NEIGHBOURS_REPLY:
		return Message_Handler::Action::SEND_MESSAGE;
	case::Message::MessageType::ERROR_MESSAGE:
		return Message_Handler::Action::RESEND;
	case Message::MessageType::CONFIRM_MESSAGE:
		return Message_Handler::Action::NO_ACTION;
	case Message::MessageType::REQ_PING:
		if (buffer[1] << 8 + buffer[2] == Variables::ID || buffer[1] << 8 + buffer[2] == Variables::BROADCAST_ID)
		{
			message = new uint8_t[Message::MESSAGE_LENGTH];
			ConfirmPingMessage m(Variables::END_ID, Variables::ID, Variables::ID, Variables::TTL);
			m.gen_message_string(message);
			if (bridge_list.push_back(message) < 0)
				delete message;
			message = new uint8_t[Message::MESSAGE_LENGTH];
			for (uint8_t i = 0; i < Message::MESSAGE_LENGTH; ++i) {
				message[i] = buffer[i];
			}
			if (bridge_list.push_back(message) < 0)
				delete message;
			if (buffer[1] << 8 + buffer[2] == Variables::BROADCAST_ID)
			{
				message = new uint8_t[Message::MESSAGE_LENGTH];
				for (uint8_t i = 0; i < Message::MESSAGE_LENGTH; ++i) {
					message[i] = buffer[i];
				}
				if (bridge_list.push_back(message) < 0)
					delete message;
			}
			return Message_Handler::Action::CONFIRM;
		}
	default:
		message = new uint8_t[Message::MESSAGE_LENGTH];
		for (uint8_t i = 0; i < Message::MESSAGE_LENGTH; ++i) {
			message[i] = buffer[i];
		}
		if (bridge_list.push_back(message) < 0)
			delete message;
		return Message_Handler::Action::CONFIRM;
	}
	return Message_Handler::Action::NO_ACTION;
}

uint8_t Message_Handler::send_message(uint8_t buffer[], Message_Handler::Source source)
{
	return source == Message_Handler::Source::BRIDGE ? send_bridge_message(buffer) : send_group_message(buffer);
}

uint8_t Message_Handler::send_bridge_message(uint8_t buffer[])
{
	if (Variables::DEBUG) {
		Serial.print("Messages for the master: ");
		Serial.println(bridge_list.size());
	}
	if (bridge_list.size() <= 0) 
	{
		return -1;
	}
	else
	{
		uint8_t *message = bridge_list.pop_front();
		for (uint8_t i = 0; i < Message::MESSAGE_LENGTH; ++i)
		{
			buffer[i] = message[i];
		}
		delete[] message;
		return Message::MESSAGE_LENGTH;
	}
}

uint8_t Message_Handler::send_group_message(uint8_t buffer[])
{
	if (message_list.size() <= 0)
	{
		return -1;
	}
	else
	{
		Serial.print("List size: ");
		Serial.println(message_list.size());
		uint8_t *message = message_list.pop_front();
		for (uint8_t i = 0; i < Message::MESSAGE_LENGTH; ++i)
		{
			buffer[i] = message[i];
		}
		delete[] message;
		return Message::MESSAGE_LENGTH;
	}
}

uint8_t Message_Handler::available_messages(Message_Handler::Source source)
{
	return source == Message_Handler::Source::BRIDGE ? bridge_list.size() : message_list.size();
}