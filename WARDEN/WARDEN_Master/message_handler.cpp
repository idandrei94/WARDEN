#include "message_handler.h"

Message_Handler::Message_Handler(uint8_t _list_size) : message_list(_list_size) {}

uint8_t Message_Handler::send_message(uint8_t *buffer) {
	if (current_message == nullptr) {
		return -1;
	}
	for (uint8_t i = 0; i < Message::MESSAGE_LENGTH; ++i)
		buffer[i] = current_message[i];
	delete[] current_message;
	current_message = nullptr;
	if (Variables::DEBUG) {
		Serial.println("Prepared a message to send");
		switch (Message::convert_type(buffer[0])) {
		case Message::MessageType::ACCIDENT:
			Serial.println("Message type: ACCIDENT");
			break;
		case Message::MessageType::CALL_NEIGHBOURS:
			Serial.println("Message type: CALL_NEIGHBOURS");
			break;
		case Message::MessageType::CALL_NEIGHBOURS_REPLY:
			Serial.println("Message type: CALL_NEIGHBOURS_REPLY");
			break;
		case Message::MessageType::CANCEL_ACCIDENT:
			Serial.println("Message type: CANCEL_ACCIDENT");
			break;
		case Message::MessageType::CONFIRM_MESSAGE:
			Serial.println("Message type: CONFIRM_MESSAGE");
			break;
		case Message::MessageType::CONFIRM_PING:
			Serial.println("Message type: CONFIRM_PING");
			break;
		case Message::MessageType::ERROR_MESSAGE:
			Serial.println("Message type: ERROR_MESSAGE");
			break;
		case Message::MessageType::REQ_PING:
			Serial.println("Message type: REQ_PING");
			break;
		default:
			Serial.print("INVALID MESSAGE TYPE: ");
			Serial.println(buffer[0]);
			break;
		}
	}
	return Message::MESSAGE_LENGTH;
}

Message_Handler::Action Message_Handler::receive_message(uint8_t *buffer) {
	if (current_message != nullptr) {
		delete[] current_message;
	}
	
	if (!Message::verify_checksum(buffer)) {
		if (Variables::DEBUG) {
			Serial.println("Message integrity fail");
		}
		return Message_Handler::Action::ERROR;
	}

	if (Variables::DEBUG) {
		switch (Message::convert_type(buffer[0])) {
		case Message::MessageType::ACCIDENT:
			Serial.println("Message type: ACCIDENT");
			break;
		case Message::MessageType::CALL_NEIGHBOURS:
			Serial.println("Message type: CALL_NEIGHBOURS");
			break;
		case Message::MessageType::CALL_NEIGHBOURS_REPLY:
			Serial.println("Message type: CALL_NEIGHBOURS_REPLY");
			break;
		case Message::MessageType::CANCEL_ACCIDENT:
			Serial.println("Message type: CANCEL_ACCIDENT");
			break;
		case Message::MessageType::CONFIRM_MESSAGE:
			Serial.println("Message type: CONFIRM_MESSAGE");
			break;
		case Message::MessageType::CONFIRM_PING:
			Serial.println("Message type: CONFIRM_PING");
			break;
		case Message::MessageType::ERROR_MESSAGE:
			Serial.println("Message type: ERROR_MESSAGE");
			break;
		case Message::MessageType::REQ_PING:
			Serial.println("Message type: REQ_PING");
			break;
		default:
			Serial.print("INVALID MESSAGE TYPE: ");
			Serial.println(buffer[0]);
			break;
		}
	}
	switch (Message::convert_type(buffer[0])) {
	case Message::MessageType::ERROR_MESSAGE:
		return Message_Handler::Action::RESEND;

	case Message::MessageType::REQ_PING:
	{
		if (check_history(buffer))
			return Message_Handler::Action::CONFIRM;
		RequestPingMessage msg(buffer);
		current_message = new uint8_t[Message::MESSAGE_LENGTH];
		if (msg.send_to == Variables::ID) {
			ConfirmPingMessage(
				Variables::END_ID,
				Variables::ID,
				Variables::ID,
				Variables::TTL).gen_message_string(current_message);
		}
		else if (msg.TTL > 0) {
			push_to_history(buffer);
			RequestPingMessage(
				msg.send_to,
				msg.msg_source,
				Variables::ID,
				msg.TTL - 1).gen_message_string(current_message);
		}
		return Message_Handler::Action::CONFIRM;
	}
	case Message::MessageType::CONFIRM_PING:
	{
		if (check_history(buffer))
			return Message_Handler::Action::CONFIRM;
		ConfirmPingMessage msg(buffer);
		if (msg.TTL > 0) {
			current_message = new uint8_t[Message::MESSAGE_LENGTH];
			ConfirmPingMessage(
				msg.send_to,
				msg.msg_source,
				Variables::ID,
				msg.TTL - 1).gen_message_string(current_message);
		}
		return Message_Handler::Action::CONFIRM;
	}

	case Message::MessageType::ACCIDENT:
	{
		if (check_history(buffer))
			return Message_Handler::Action::CONFIRM;
		AccidentMessage msg(buffer);
		if (msg.TTL > 0) {
			current_message = new uint8_t[Message::MESSAGE_LENGTH];
			AccidentMessage(
				msg.send_to,
				msg.msg_source,
				Variables::ID,
				msg.TTL - 1,
				buffer + 8).gen_message_string(current_message);
		}
		return Message_Handler::Action::CONFIRM;
	}

	case Message::MessageType::CANCEL_ACCIDENT:
	{
		if (check_history(buffer))
			return Message_Handler::Action::CONFIRM;
		CancelAccidentMessage msg(buffer);
		if (msg.TTL > 0) {
			current_message = new uint8_t[Message::MESSAGE_LENGTH];
			CancelAccidentMessage(
				msg.send_to,
				msg.msg_source,
				Variables::ID,
				msg.TTL - 1,
				buffer + 8).gen_message_string(buffer); // Doing buffer+8 is horrible but since we're mirroring 
		}                                               // a fraction of the buffer we're good
		return Message_Handler::Action::CONFIRM;
	}
	case Message::MessageType::CALL_NEIGHBOURS_REPLY:
		return Message_Handler::Action::PROCEED;
	case Message::MessageType::CONFIRM_MESSAGE:
		return Message_Handler::Action::PROCEED;
	default:
		return Message_Handler::Action::NO_ACTION;
	}
}

uint8_t Message_Handler::count() {
	return message_list.size();
}

uint8_t Message_Handler::push_to_history(uint8_t buffer[]) {
	if (message_list.capacity() == message_list.size()) {
		delete message_list.pop_front();
	}
	uint8_t *message = new uint8_t[Message::MESSAGE_LENGTH];
	for (uint8_t i = 0; i < Message::MESSAGE_LENGTH; ++i)
	{
		message[i] = buffer[i];
	} 
	message_list.push_back(message);
	return message_list.size();
}

bool Message_Handler::check_history(uint8_t buffer[]) {
	for (uint8_t i = 0; i < message_list.size(); ++i) {
		if ((message_list.get(i))[0] == buffer[0]) {                           // Same type
			if ((message_list.get(i))[1] == buffer[1])                         // Same destination
				if ((message_list.get(i))[2] == buffer[2])                  
					if ((message_list.get(i))[5] == buffer[5])                 // Same source
						if ((message_list.get(i))[6] == buffer[6]) {
							for (uint8_t j = 8; j < 55; ++j)          // Same data
								if ((message_list.get(i))[j] != buffer[j])
									return false;
							return true;
						}
		}
	}
	return false;
}