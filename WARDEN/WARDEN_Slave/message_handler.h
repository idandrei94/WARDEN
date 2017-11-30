#pragma once

#include "list.h"
#include "message.h"


class Message_Handler
{
public:

	enum class Action {
		ERROR,
		NO_ACTION,
		SEND_MESSAGE,
		CONFIRM,
		REPLY,
		RESEND
	};

	enum class Source {
		BRIDGE,
		GROUP
	};

	enum class State {
		CONNECTED,
		FREE
	};

private:

	/**	bridge_list		The queue containing messages to be sent through the bridge
	*	message_list	The queue containing messages for the master
	*	slave_state			The slave state - currently talking to the master or not
	*/
	List<uint8_t*> bridge_list;
	List<uint8_t*> message_list;
	Message_Handler::State slave_state;


	// Handle a message from the master
	Message_Handler::Action receive_bridge_message(uint8_t buffer[]);
	// Handle a message from the bridge
	Message_Handler::Action receive_group_message(uint8_t buffer[]);

	// Get a message to send from the bridge to the master
	uint8_t send_bridge_message(uint8_t buffer[]);
	// Get a message to send from the master to the bridge
	uint8_t send_group_message(uint8_t buffer[]);

public:

	/** The constructor initializes the queue capacity
	*	@param list_size	The capacity of the queue
	*/
	Message_Handler(uint8_t _list_size);

	/** Handles a newly received message. Generates message replies to be obtained through the send_message function
	*	@pre				The buffer must contain a valid WARDEN message
	*	@post				The buffer will contain the immediate reply if necessary
	*	@return				Returns an Action to be taken
	*/
	Message_Handler::Action receive_message(uint8_t buffer[], Message_Handler::Source source);

	/** Send the current message
	*	@pre				The buffer must be large enough to contain Message::MESSAGE_LENGTH
	*	@post				The buffer will contain the message string if available
	*	@return				Message::MESSAGE_LENGTH if a message is available, -1 otherwise
	*/
	uint8_t send_message(uint8_t buffer[], Message_Handler::Source source);

	uint8_t available_messages(Message_Handler::Source source);
};