#include "endpoint.h"
#include "system_variables.h"

Endpoint::Endpoint() : 
	driver(HC12_Driver::HC12_Modes::GROUP), 
	current_neighbour(0), 
	call_msg(), 
	handler(Variables::LIST_SIZE),
	s_driver()
{

}

void Endpoint::action() {
	if (Variables::DEBUG) {
		Serial.print("Calling neighbour #");
		Serial.println(current_neighbour);
	}
	s_driver.read();
	if (Variables::ENABLED == false)
		return;
	uint16_t ping_id = 0x00;
	if (s_driver.get_id(ping_id))
	{
		RequestPingMessage(ping_id, Variables::ID, Variables::ID, Variables::TTL).gen_message_string(send_buffer);
		forwardToNeighbours(current_neighbour);
	}
	if (current_neighbour == 0)
	{

		if (handler.send_message(send_buffer) == Message::MESSAGE_LENGTH)
		{
			forwardToNeighbours(current_neighbour);
		}
	}
	driver.send_message(call_msg.gen_message_string(current_neighbour, false), Message::MESSAGE_LENGTH); //Call neighbour
	if (driver.get_message(receive_buffer, Message::MESSAGE_LENGTH) == Message::MESSAGE_LENGTH) // HC12 received a message
	{
		if (Variables::DEBUG) {
			Serial.println("Received reply");
		}
		Message_Handler::Action action = handler.receive_message(receive_buffer);
		switch (action) {
		case Message_Handler::Action::CONFIRM:
			ConfirmMessage((receive_buffer[3] << 8) + receive_buffer[4]).gen_message_string(send_buffer);
			driver.send_message(send_buffer, Message::MESSAGE_LENGTH);
			Serial.write(receive_buffer, Message::MESSAGE_LENGTH);
			break;
		case Message_Handler::Action::ERROR:
			ErrorMessage((receive_buffer[3] << 8) + receive_buffer[4]).gen_message_string(send_buffer);
			driver.send_message(send_buffer, Message::MESSAGE_LENGTH);
			break;
		case Message_Handler::Action::NO_ACTION:
			break;
		case Message_Handler::Action::RESEND:
			driver.send_message(send_buffer, Message::MESSAGE_LENGTH);
			break;
		default:
			break;
		}
	}
	else
	{
		if (Variables::DEBUG) {
			Serial.println("  No reply");
		}
	}
	current_neighbour = (current_neighbour + 1) % 5;
}

void Endpoint::forwardToNeighbours(uint8_t current_neighbour) {
	for (uint8_t neighbour = 0; neighbour < 5; ++neighbour)
	{
		if (neighbour != current_neighbour)// Not sending back to the current neighbour
		{
			driver.send_message(call_msg.gen_message_string(neighbour, true), Message::MESSAGE_LENGTH);
			if (driver.get_message(receive_buffer, Message::MESSAGE_LENGTH) == Message::MESSAGE_LENGTH) // HC12 received a message
			{
				Message_Handler::Action action = handler.receive_message(receive_buffer);
				switch (action)
				{
				case Message_Handler::Action::PROCEED:
					do {
						driver.send_message(send_buffer, Message::MESSAGE_LENGTH);
					} while (driver.get_message(receive_buffer, Message::MESSAGE_LENGTH) &&
						handler.receive_message(receive_buffer) != Message_Handler::Action::PROCEED);// Keep sending until you receive CONFIRM
					break;
				default:
					break;
				}
			}
		}
	}
}