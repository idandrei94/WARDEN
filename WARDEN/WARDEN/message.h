#pragma once
#include <Arduino.h>
#include <stdint.h>
#include "system_variables.h"

class Message {
public:
	enum class MessageType {
		CALL_NEIGHBOURS = 0x01,
		CALL_NEIGHBOURS_REPLY = 0x02,
		ERROR_MESSAGE = 0x03,
		CONFIRM_MESSAGE = 0x04,
		REQ_PING = 0x05,
		CONFIRM_PING = 0x06,
		ACCIDENT = 0x07,
		CANCEL_ACCIDENT = 0x08
	};
	static const uint8_t MESSAGE_LENGTH = 64;

	/** Converts the enum MessagteType to its byte equivalent for the byte string
	* @param type		    A MessageType member
	* @return				Returns the byte equivalent of the MessageType argument
	*/
	static uint8_t convert_type(MessageType type);

	/** Converts the byte message type to its MessageType member equivalent
	* @param type		    The bmessage type in byte form
	* @pre					The argument must originally be generated through the 
	*							convert_type(MessageType) : uint8_t function
	* @return				Returns the equivalent MessageType member
	*/
	static MessageType convert_type(uint8_t type);

	/** Generates the message checksum, excludes the TTL and received_from fields
	* @param buffer		    The buffer containing the message
	* @pre					The buffer must have at least Message::MESSAGE_LENGTH bytes of memory
	* @post					The last 9 bytes in the buffer will contain the checksum
	* @return				Returns the length of the checksum, -1 if an error occurs
	*/
	static uint8_t generate_checksum(uint8_t buffer[]);

	/** Verifies the message integrity
	* @param buffer		    The buffer containing the message
	* @pre					The buffer must contain a message with its initial checksum
	* @return				Returns true if the checksums match, false otherwise
	*/
	static bool verify_checksum(uint8_t buffer[]);

	/** 
	*	type				The type of the message
	*	send_to				The WARDEN unit ID the message is addressed to
	*	received_from		The ID of the unit the message was received from
	*	msg_source			The ID of the unit that generated the message
	*	TTL					Time-to-live of the message, how many times it will be forwarded by the units
	*/
	const MessageType type;
	const uint16_t send_to;
	const uint16_t received_from;
	const uint16_t msg_source;
	const uint8_t TTL;

	/** Generates the byte string for this message to send via HC12
	*						[TYPE][SEND_TO][SOURCE][RECEIVED_FROM][DATA][CHECKSUM]
	*	@param buffer		The buffer where the string will be copied to
	*	@pre				The argument buffer must be at least Message::MESSAGE_LENGTH bytes
	*	@post				The argument will contain the byte string equivalent to the message
	*	@return				Returns the length of the message if successful, -1 otherwise
	*/
	virtual uint8_t gen_message_string(uint8_t *buffer);
protected:
	/** 
	*   @param _send_to				The WARDEN unit ID the message is addressed to
	*	@param _received_from		The ID of the unit the message was received from
	*	@param _msg_source			The ID of the unit that generated the message
	*	@param _TTL					Time-to-live of the message, how many times it will be forwarded by the units
	*/
	Message(MessageType _type, uint16_t _send_to, uint16_t _received_from, uint16_t _msg_source, uint8_t _TTL) :
		type(_type),
		send_to(_send_to),
		received_from(_received_from),
		msg_source(_msg_source),
		TTL(_TTL) {};
};

class CallNeighboursMessage {
public:
	CallNeighboursMessage();
	/**
	*	@param neighbour_id		The id of the neighbour to be called. Must range from 1 to 5
	*	@return					The message string
	*/
	uint8_t *gen_message_string(uint8_t neighbour_id, bool give);
private:
	uint8_t message[64];
};

class ConfirmMessage : public Message {
public:
	ConfirmMessage(uint16_t _send_to);
};

class RequestPingMessage : public Message {
public:
	RequestPingMessage(uint16_t _send_to, uint16_t _msg_source, uint16_t _received_from, uint8_t _TTL);
	RequestPingMessage(uint8_t *buffer);
};

class ConfirmPingMessage : public Message {
public:
	ConfirmPingMessage(uint16_t _send_to, uint16_t _msg_source, uint16_t _received_from, uint8_t _TTL);
	ConfirmPingMessage(uint8_t *buffer);
};

class AccidentMessage : public Message {
public:
	/**
	*	@pre	the data buffer must be the size of the Message data length (48 bytes)
	*/
	AccidentMessage(uint16_t _send_to, uint16_t _msg_source, uint16_t _received_from, uint8_t _TTL, uint8_t *_data);
	AccidentMessage(uint8_t *buffer);
	uint8_t gen_message_string(uint8_t *buffer);
private:
	uint8_t data[48];
};

class CancelAccidentMessage : public Message {
public:
	/**
	*	@pre	the data buffer must be the size of the Message data length
	*/
	CancelAccidentMessage(uint16_t _send_to, uint16_t _msg_source, uint16_t _received_from, uint8_t _TTL, uint8_t *_data);
	CancelAccidentMessage(uint8_t *buffer);
	uint8_t gen_message_string(uint8_t *buffer);
private:
	uint8_t data[48];
};

class ErrorMessage : public Message {
public:
	ErrorMessage(uint16_t _send_to);
};

class CallNeighbourReplyMessage : public Message {
public:
	CallNeighbourReplyMessage(uint16_t _send_to, uint8_t _status);
	uint8_t gen_message_string(uint8_t *buffer);
private:
	uint8_t status;
};