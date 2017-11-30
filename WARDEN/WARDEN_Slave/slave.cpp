#include "slave.h"
#include "system_variables.h"

Slave::Slave() :
	group_driver(new HC12_Driver(HC12_Driver::HC12_Modes::GROUP, 100)),
	bridge_driver(new HC12_Driver(HC12_Driver::HC12_Modes::BRIDGE, 50)),
	call_msg(),
	handler(new Message_Handler(Variables::LIST_SIZE)),
	group_bridge_switch(true)
{
}

void Slave::action()
{
	if (group_bridge_switch) // Listen group once, then listen on the bridge once
	{
		if (group_driver->get_message(group_receive_buffer, Message::MESSAGE_LENGTH) == Message::MESSAGE_LENGTH)
		{
			switch (handler->receive_message(group_receive_buffer, Message_Handler::Source::GROUP))
			{
			case  Message_Handler::Action::CONFIRM:
				ConfirmMessage((group_receive_buffer[3] << 8) + group_receive_buffer[4]).gen_message_string(group_send_buffer);
				group_driver->send_message(bridge_send_buffer, Message::MESSAGE_LENGTH);
				break;
			case  Message_Handler::Action::ERROR:
				ErrorMessage((group_receive_buffer[3] << 8) + group_receive_buffer[4]).gen_message_string(group_send_buffer);
				group_driver->send_message(bridge_send_buffer, Message::MESSAGE_LENGTH);
				break;
			case  Message_Handler::Action::REPLY:
				break;
			case  Message_Handler::Action::RESEND:
				group_driver->send_message(bridge_send_buffer, Message::MESSAGE_LENGTH);
				break;
			case  Message_Handler::Action::SEND_MESSAGE:
				if (handler->send_message(group_send_buffer, Message_Handler::Source::BRIDGE) == Message::MESSAGE_LENGTH)
				{
					group_driver->send_message(group_send_buffer, Message::MESSAGE_LENGTH);
				}
				break;
			default:
				break;
			}
		}
	}
	else
	{
		if (bridge_driver->get_message(bridge_receive_buffer, Message::MESSAGE_LENGTH) == Message::MESSAGE_LENGTH)
		{
			Serial.println("Bridge message receved");
			switch (handler->receive_message(group_receive_buffer, Message_Handler::Source::BRIDGE))
			{
			case Message_Handler::Action::CONFIRM:
				ConfirmMessage((bridge_receive_buffer[3] << 8) + bridge_receive_buffer[4]).gen_message_string(bridge_send_buffer);
				bridge_driver->send_message(bridge_send_buffer, Message::MESSAGE_LENGTH);
				break;
			case Message_Handler::Action::ERROR:
				ErrorMessage((bridge_receive_buffer[3] << 8) + bridge_receive_buffer[4]).gen_message_string(bridge_send_buffer);
				bridge_driver->send_message(bridge_send_buffer, Message::MESSAGE_LENGTH);
				break;
			case Message_Handler::Action::REPLY:
			{
				CallNeighbourReplyMessage reply(0, 0);
				reply.gen_message_string(bridge_send_buffer);
				bridge_driver->send_message(bridge_send_buffer, Message::MESSAGE_LENGTH);
				break;
			}
			case Message_Handler::Action::SEND_MESSAGE:
				if (handler->send_message(bridge_send_buffer, Message_Handler::Source::GROUP) == Message::MESSAGE_LENGTH)
				{
					bridge_driver->send_message(bridge_send_buffer, Message::MESSAGE_LENGTH);
				}
				break;
			case Message_Handler::Action::RESEND:
				bridge_driver->send_message(bridge_send_buffer, Message::MESSAGE_LENGTH);
				break;
			default:
				break;
			}
		}
		else // Try to send a message to the other slave
		{
			if(handler->available_messages(Message_Handler::Source::GROUP))
				bridge_driver->send_message(call_msg.gen_message_string(0, false), Message::MESSAGE_LENGTH);
		}
	}
	group_bridge_switch = !group_bridge_switch;
}