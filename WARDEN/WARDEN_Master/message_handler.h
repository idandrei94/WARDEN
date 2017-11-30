#pragma once

#include "message.h"
#include "list.h"
#include "system_variables.h"

class Message_Handler {
private:
	/**	The queue containing messages to be sent
	*/
	List<uint8_t*> message_list;
	uint8_t* current_message = nullptr;

public:
	enum class Action {
		NO_ACTION,
		RESEND,
		CONFIRM, 
		PROCEED,
		ERROR
	};

	/** The constructor initializes the queue capacity
	*	@param list_size	The capacity of the queue
	*/
	Message_Handler(uint8_t _list_size);

	/** Handles a newly received message. Generates message replies to be obtained through the send_message function
	*	@pre				The buffer must contain a valid WARDEN message
	*	@post				The buffer will contain the immediate reply if necessary
	*	@return				Returns an Action to be taken
	*/
	Message_Handler::Action receive_message(uint8_t buffer[]);

	/** Send the current message
	*	@pre				The buffer must be large enough to contain Message::MESSAGE_LENGTH
	*	@post				The buffer will contain the message string if available
	*	@return				Message::MESSAGE_LENGTH if a message is available, -1 otherwise
	*/
	uint8_t send_message(uint8_t buffer[]);

	/** Returns the message queue count
	*	@return				The number of messages in the queue
	*/
	uint8_t count();


	/**	Push a message to the message history queue
	*	@param buffer		The buffer containing the message
	*	@pre				The buffer contains a valid WARDEN message
	*	@post				The message will be added to the end of the list. If the list was full
	*							prior to adding, first element will be popped first.
	*	@return				The count of the message list
	*/
	uint8_t push_to_history(uint8_t buffer[]);

	/**	Check if a message has already been send recently. Only checks the source, destination and data fields
	*	@param buffer		Contains the message
	*	@pre				The argument contains a valid WARDEN message
	*	@return				Returns whether the message exists in the history queue or not
	*/
	bool check_history(uint8_t buffer[]);
};