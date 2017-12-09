#include "message.h"

uint8_t Message::convert_type(Message::MessageType type) {
	switch (type) {
	case Message::MessageType::CALL_NEIGHBOURS:
		return 0x01;
	case Message::MessageType::CALL_NEIGHBOURS_REPLY:
		return 0x02;
	case Message::MessageType::ERROR_MESSAGE:
		return 0x03;
	case Message::MessageType::CONFIRM_MESSAGE:
		return 0x04;
	case Message::MessageType::REQ_PING:
		return 0x05;
	case Message::MessageType::CONFIRM_PING:
		return 0x06;
	case Message::MessageType::ACCIDENT:
		return 0x07;
	case Message::MessageType::CANCEL_ACCIDENT:
		return 0x08;
	}
}

Message::MessageType Message::convert_type(uint8_t type) {
	switch (type) {
	case 0x01:
		return Message::MessageType::CALL_NEIGHBOURS;
	case 0x02:
		return Message::MessageType::CALL_NEIGHBOURS_REPLY;
	case 0x03:
		return Message::MessageType::ERROR_MESSAGE;
	case 0x04:
		return Message::MessageType::CONFIRM_MESSAGE;
	case 0x05:
		return Message::MessageType::REQ_PING;
	case 0x06:
		return Message::MessageType::CONFIRM_PING;
	case 0x07:
		return Message::MessageType::ACCIDENT;
	case 0x08:
		return Message::MessageType::CANCEL_ACCIDENT;
	}
}

// Reverses the bits in a 32bit value, used for generating the message checksum
uint32_t reverse(uint32_t x) {
	x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
	x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
	x = ((x & 0x0F0F0F0F) << 4) | ((x >> 4) & 0x0F0F0F0F);
	x = (x << 24) | ((x & 0xFF00) << 8) |
		((x >> 8) & 0xFF00) | (x >> 24);
	return x;
}

// implement message integrity
uint8_t Message::generate_checksum(uint8_t buffer[]) {
	for (uint8_t i = 0; i < 9; ++i) {
		buffer[63 - i] = 0x00;
	}
	int i, j;
	uint32_t crc, mask;
	uint8_t byte;

	i = 0;
	crc = 0xFFFFFFFF;
	while (i<Message::MESSAGE_LENGTH-9) {
		byte = buffer[i];            // Get next byte.
		crc = crc ^ byte;
		for (j = 7; j >= 0; j--) {    // Do eight times.
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
		++i;
	}
	crc = ~crc;
	for(i=0; i<4;++i)
		buffer[Message::MESSAGE_LENGTH - i-1] = static_cast<uint8_t>(crc>>(8*i));
	return Message::MESSAGE_LENGTH;
}

// implement message integrity
bool Message::verify_checksum(uint8_t buffer[])
{
	uint8_t old_checksum[9];                   // Store the received checksum
	uint8_t i = 0;
	for (; i < 9; ++i) {
		old_checksum[i] = buffer[55 + i];
	}
	Message::generate_checksum(buffer);        // Generate the checksum again
	for (; i >= 0; --i) {                      // Compare checksums
		if (old_checksum[i] != buffer[55 + i])
		{
			return false;
		}
	}
	return true;
}

uint8_t Message::gen_message_string(uint8_t buffer[])
{
	buffer[0] = Message::convert_type(type);
	buffer[1] = send_to >> 8;                       // Top half
	buffer[2] = static_cast<uint8_t>(send_to);      // Bottom half
	buffer[3] = received_from >> 8;
	buffer[4] = static_cast<uint8_t>(received_from);
	buffer[5] = msg_source >> 8;
	buffer[6] = static_cast<uint8_t>(msg_source);
	buffer[7] = TTL;
	for (uint8_t i = 8; i <55; ++i) {
		buffer[i] = 0x0;
	}
	return Message::generate_checksum(buffer);
}

ConfirmMessage::ConfirmMessage(uint16_t _send_to) : Message
(
	Message::MessageType::CONFIRM_MESSAGE,
	_send_to,
	Variables::ID,
	Variables::ID,
	1
)
{
}

RequestPingMessage::RequestPingMessage(uint16_t _send_to, uint16_t _msg_source, uint16_t _received_from, uint8_t _TTL)
	: Message(
		Message::MessageType::REQ_PING,
		_send_to,
		_received_from,
		_msg_source,
		_TTL
	)
{
}

RequestPingMessage::RequestPingMessage(uint8_t buffer[]) : Message
(
	Message::convert_type(buffer[0]),
	buffer[1] << 8 + buffer[2],
	buffer[3] << 8 + buffer[4],
	buffer[5] << 8 + buffer[6],
	buffer[7]
)
{
}

ConfirmPingMessage::ConfirmPingMessage(uint16_t _send_to, uint16_t _msg_source, uint16_t _received_from, uint8_t _TTL) : Message(
	Message::MessageType::CONFIRM_PING,
	_send_to,
	_received_from,
	_msg_source, 
	_TTL
)
{}

ConfirmPingMessage::ConfirmPingMessage(uint8_t buffer[]): Message
(
	Message::convert_type(buffer[0]),
		buffer[1] << 8 + buffer[2],
		buffer[3] << 8 + buffer[4],
		buffer[5] << 8 + buffer[6],
		buffer[7]
)
{}

AccidentMessage::AccidentMessage(uint16_t _send_to, uint16_t _msg_source, uint16_t _received_from, uint8_t _TTL, uint8_t *_data) : Message(
	Message::MessageType::ACCIDENT,
	_send_to,
	_received_from,
	_msg_source,
	_TTL
)
{
	for (uint8_t i = 0; i < 48; ++i) {
		data[i] = _data[i];
	}
}

AccidentMessage::AccidentMessage(uint8_t buffer[]) : Message
(
	Message::convert_type(buffer[0]),
	buffer[1] << 8 + buffer[2],
	buffer[3] << 8 + buffer[4],
	buffer[5] << 8 + buffer[6],
	buffer[7]
)
{
	for (uint8_t i = 8; i < 55; ++i) {
		data[i - 8] = buffer[i];
	}
}

uint8_t AccidentMessage::gen_message_string(uint8_t buffer[])
{
	buffer[0] = Message::convert_type(type);
	buffer[1] = send_to >> 8;                       // Top half
	buffer[2] = static_cast<uint8_t>(send_to);      // Bottom half
	buffer[3] = received_from >> 8;
	buffer[4] = static_cast<uint8_t>(received_from);
	buffer[5] = msg_source >> 8;
	buffer[6] = static_cast<uint8_t>(msg_source);
	buffer[7] = TTL;
	for (uint8_t i = 8; i < 55; ++i) {
		buffer[i] = data[i];
	}
	return Message::generate_checksum(buffer);
}

CancelAccidentMessage::CancelAccidentMessage(uint16_t _send_to, uint16_t _msg_source, uint16_t _received_from, uint8_t _TTL, uint8_t *_data) : Message(
	Message::MessageType::CANCEL_ACCIDENT,
	_send_to,
	_received_from,
	_msg_source,
	_TTL
)
{
	for (uint8_t i = 0; i < 48; ++i) {
		data[i] = _data[i];
	}
}

CancelAccidentMessage::CancelAccidentMessage(uint8_t buffer[]) : Message
(
	Message::convert_type(buffer[0]),
	buffer[1] << 8 + buffer[2],
	buffer[3] << 8 + buffer[4],
	buffer[5] << 8 + buffer[6],
	buffer[7]
)
{
	for (uint8_t i = 8; i < 55; ++i) {
		data[i - 8] = buffer[i];
	}
}

uint8_t CancelAccidentMessage::gen_message_string(uint8_t buffer[]) {
	buffer[0] = Message::convert_type(type);
	buffer[1] = send_to >> 8;                       // Top half
	buffer[2] = static_cast<uint8_t>(send_to);      // Bottom half
	buffer[3] = received_from >> 8;
	buffer[4] = static_cast<uint8_t>(received_from);
	buffer[5] = msg_source >> 8;
	buffer[6] = static_cast<uint8_t>(msg_source);
	buffer[7] = TTL;
	for (uint8_t i = 0; i < 48; ++i) {
		buffer[i + 8] = data[i];
	}
	return Message::generate_checksum(buffer);
}

ErrorMessage::ErrorMessage(uint16_t _send_to) : Message(
	Message::MessageType::ERROR_MESSAGE,
	_send_to,
	Variables::ID,
	Variables::ID,
	1
)
{}

CallNeighbourReplyMessage::CallNeighbourReplyMessage(uint16_t _send_to, uint8_t _status) : Message(
	Message::MessageType::CALL_NEIGHBOURS_REPLY,
	_send_to,
	Variables::ID,
	Variables::ID,
	1
), status(_status)
{}

uint8_t  CallNeighbourReplyMessage::gen_message_string(uint8_t buffer[]) {
	buffer[0] = Message::convert_type(type);
	buffer[1] = send_to >> 8;                       // Top half
	buffer[2] = static_cast<uint8_t>(send_to);      // Bottom half
	buffer[3] = received_from >> 8;
	buffer[4] = static_cast<uint8_t>(received_from);
	buffer[5] = msg_source >> 8;
	buffer[6] = static_cast<uint8_t>(msg_source);
	buffer[7] = status;
	for (uint8_t i = 8; i < 64; ++i) {
		buffer[i + 7] = 0x00;
	}
	return Message::generate_checksum(buffer);
}

CallNeighboursMessage::CallNeighboursMessage()
{
	message[0] = Message::convert_type(Message::MessageType::CALL_NEIGHBOURS);
	message[1] = Variables::BROADCAST_ID >> 8;                       // Top half
	message[2] = static_cast<uint8_t>(Variables::BROADCAST_ID);      // Bottom half
	message[3] = Variables::ID >> 8;
	message[4] = static_cast<uint8_t>(Variables::ID);
	message[5] = Variables::ID >> 8;
	message[6] = static_cast<uint8_t>(Variables::ID);
	for (uint8_t i = 7; i < 48; ++i) {
		message[i + 7] = 0x00;
	}
}

uint8_t* CallNeighboursMessage::gen_message_string(uint8_t neighbour, bool give) {
	message[7] = neighbour;
	message[8] = give ? 1 : 0;
	Message::generate_checksum(message);
	return message;
}