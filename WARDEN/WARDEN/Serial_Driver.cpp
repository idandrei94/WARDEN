#include "Serial_Driver.h"
#include "Persistence.h"
#include <Arduino.h>
#include "system_variables.h"

Serial_Driver::Serial_Driver()
{
	Serial.begin(9600);
	while (Serial.available())
		Serial.read();
}

uint8_t Serial_Driver::read()
{
	if (Serial.available())
	{
		char c = Serial.read();
		buffer[count++] = c;
		if (c == '!')
		{
			parse_serial_message();
			count = 0;
		}
		else if (count == 12) // If we have read 12 characters but not a valid end-of-message
		{                     // Wait 1 second and clear buffer
			Serial.println("ERROR+MSG!");
			delay(1000);
			while (Serial.available())
				Serial.read();
			strcpy(buffer, "###########");
			count = 0;
		}
	}
	return count;
}

void Serial_Driver::parse_serial_message()
{
	if (strncmp(buffer, "WRD+", 4) == 0) // All commands start with "WRD+"
	{
		char *str = buffer+4; // Skip over the "WRD+" part
		if (strncmp(str, "DBG", 3) == 0) // Debug command
		{
			str += 3;
			if (str[0] == '!') //Empty command, we return the current setting
			{
				Serial.print("OK+DBG");
				Serial.print(Variables::DEBUG ? 1 : 0);
				Serial.println("!");
			}
			else if (str[1] == '!')
			{
				uint8_t value = str[0] - '0';
				if (value == 0 || value == 1)
				{
					set_debug(value);
					Serial.print("OK+DBG");
					Serial.print(Variables::DEBUG ? 1 : 0);
					Serial.println("!");
				}
				else
				{
					Serial.println("ERROR+DBG!");
				}
			}
		}
		else if (strncmp(str, "ID", 2) == 0)
		{
			str += 2;
			if (str[0] == '!')
			{
				char id_string[] = "0x0000";
				itoa(Variables::ID, id_string, 16);
				Serial.print("OK+ID");
				Serial.print(id_string+2);
				Serial.println("!");
			}
			else if (str[4] != '!') // The set ID format is xxxx!
			{
				Serial.println("ERROR+ID!");
			}
			else
			{
				//Parse the ID from hex string to actual number
				//An incorrect string will result in ID 0x0
				char id_string[] = "0x0000";
				id_string[2] = buffer[0];
				id_string[3] = buffer[1];
				id_string[4] = buffer[2];
				id_string[5] = buffer[3];
				set_ID(static_cast<uint16_t>(strtol(id_string, NULL, 16)));
				Serial.print("OK+ID");
				Serial.print(id_string + 2);
				Serial.println("!");
			}
		}
		else if (strncmp(str, "BID", 3) == 0)
		{
			str += 3;
			if (str[0] == '!')
			{
				char id_string[] = "0x0000";
				itoa(Variables::ID, id_string, 16);
				Serial.print("OK+BID");
				Serial.print(id_string+2);
				Serial.println("!");
			}
			else if (str[4] != '!') // The set ID format is xxxx!
			{
				Serial.println("ERROR+BID!");
			}
			else
			{
				//Parse the ID from hex string to actual number
				//An incorrect string will result in ID 0x0
				char id_string[] = "0x0000";
				id_string[2] = buffer[0];
				id_string[3] = buffer[1];
				id_string[4] = buffer[2];
				id_string[5] = buffer[3];
				set_broadcast_ID(static_cast<uint16_t>(strtol(id_string, NULL, 16)));
				Serial.print("OK+BID");
				Serial.print(id_string + 2);
				Serial.println("!");
			}
		}
		else if (strncmp(str, "EID", 3) == 0)
		{
			str += 3;
			if (str[0] == '!')
			{
				char id_string[] = "0x0000";
				itoa(Variables::ID, id_string, 16);
				Serial.print("OK+EID");
				Serial.print(id_string + 2);
				Serial.println("!");
			}
			else if (str[4] != '!') // The set ID format is xxxx!
			{
				Serial.println("ERROR+EID!");
			}
			else
			{
				//Parse the ID from hex string to actual number
				//An incorrect string will result in ID 0x0
				char id_string[] = "0x0000";
				id_string[2] = buffer[0];
				id_string[3] = buffer[1];
				id_string[4] = buffer[2];
				id_string[5] = buffer[3];
				set_endpoint_ID(static_cast<uint16_t>(strtol(id_string, NULL, 16)));
				Serial.print("OK+EID");
				Serial.print(id_string + 2);
				Serial.println("!");
			}
		}
		else if (strncmp(str, "LST", 3) == 0)
		{
			str += 3;
			if(str[0] == '!')
			{
				Serial.print("OK+LST");
				Serial.print(Variables::LIST_SIZE);
				Serial.println("!");
			}
			else if (str[3] != '!') // Set list format is xxx!
			{
				Serial.println("ERROR+LST!");
			}
			else
			{
				char list_str[] = "000";
				list_str[0] = str[0];
				list_str[1] = str[1];
				list_str[2] = str[2];
				set_list_size(static_cast<uint8_t>(strtol(list_str, NULL, 10)));
				Serial.print("OK+LST");
				Serial.print(list_str);
				Serial.println("!");
			}
		}
		else if (strncmp(str, "NID", 3) == 0)
		{
			str += 3;
			if (str[0] == '!')
			{
				Serial.print("OK+NID");
				Serial.print(Variables::NEIGHBOUR_ID);
				Serial.println("!");
			}
			else if (str[1] != '!')
			{
				Serial.println("ERROR+NID!");
			}
			else
			{
				char nid_str[] = "0";
				nid_str[0] = str[0];
				set_neighbour_ID(static_cast<uint8_t> (strtol(nid_str, NULL, 10)));
				Serial.print("OK+NID");
				Serial.print(nid_str);
				Serial.println("!");
			}
		}
		else if(strncmp(str, "GCH", 3) == 0)
		{
			str += 3;
			if (str[0] == '!')
			{
				Serial.print("OK+GCH");
				Serial.print(Variables::GROUP_CHANNEL);
				Serial.println("!");
			}
			else if (str[2] != '!')
			{
				Serial.println("ERROR+GCH!");
			}
			else
			{
				char ch_str[] = "00";
				ch_str[0] = str[0];
				ch_str[1] = str[1];
				set_group_channel(static_cast<uint8_t>(strtol(ch_str, NULL, 10)));
				Serial.print("OK+GCH");
				Serial.print(ch_str);
				Serial.println("!");
			}
		}
		else if (strncmp(str, "BCH", 3) == 0)
		{
			str += 3;
			if (str[0] == '!')
			{
				Serial.print("OK+BCH");
				Serial.print(Variables::GROUP_CHANNEL);
				Serial.println("!");
			}
			else if (str[2] != '!')
			{
				Serial.println("ERROR+BCH!");
			}
			else
			{
				char ch_str[] = "00";
				ch_str[0] = str[0];
				ch_str[1] = str[1];
				set_bridge_channel(static_cast<uint8_t>(strtol(ch_str, NULL, 10)));
				Serial.print("OK+BCH");
				Serial.print(ch_str);
				Serial.println("!");
			}
		}
		else if (strncmp(str, "TTL", 3) == 0)
		{
			str += 3;
			if (str[0] == '!')
			{
				Serial.print("OK+TTL");
				Serial.print(Variables::TTL);
				Serial.println("!");
			}
			else if (str[3] != '!')
			{
				Serial.println("ERROR+TTL!");
			}
			else
			{
				char ttl_str[] = "000";
				ttl_str[0] = str[0];
				ttl_str[1] = str[1];
				ttl_str[2] = str[2];
				set_TTL(static_cast<uint8_t>(strtol(ttl_str, NULL, 10)));
				Serial.print("OK+TTL");
				Serial.print(ttl_str);
				Serial.println("!");
			}
		}
		else if (strncmp(str, "STATUS", 6) == 0)
		{
			str += 6;
			if (str[0] == '!')
			{
				Serial.println(Variables::ENABLED ? "OK+START!" : "OK+STOP!");
			}
			else
			{
				bool option = (str[0] - '0') == 1;
				Serial.println(set_enabled(option) ? "OK+START!" : "OK+STOP!");
			}
		}
	}
	else
	{
		Serial.println("ERROR+CMD!");
	}
	reset_buffer();
}

void Serial_Driver::reset_buffer()
{
	//fill the buffer with unused character
	//to get rid of stray !'s
	strcpy(buffer, "###########");
	count = 0;
}

bool Serial_Driver::set_debug(bool debug)
{
	return Variables::DEBUG = Memory::set_debug(debug);
}

uint16_t Serial_Driver::set_ID(uint16_t ID)
{
	return Variables::ID = Memory::set_ID(ID);
}

uint16_t Serial_Driver::set_broadcast_ID(uint16_t ID)
{
	return Variables::BROADCAST_ID = Memory::set_broadcast_ID(ID);
}

uint16_t Serial_Driver::set_endpoint_ID(uint16_t ID)
{
	return Variables::END_ID = Memory::set_endpoint_ID(ID);
}

uint8_t Serial_Driver::set_list_size(uint8_t size)
{
	return Variables::LIST_SIZE = Memory::set_list_size(size);
}

uint8_t Serial_Driver::set_neighbour_ID(uint8_t ID)
{
	return Variables::NEIGHBOUR_ID = Memory::set_neighbour_ID(ID);
}

uint8_t Serial_Driver::set_group_channel(uint8_t channel)
{
	return Variables::GROUP_CHANNEL = Memory::set_group_channel(channel);
}

uint8_t Serial_Driver::set_bridge_channel(uint8_t channel)
{
	return Variables::BRIDGE_CHANNEL = Memory::set_bridge_channel(channel);
}

uint8_t Serial_Driver::set_TTL(uint8_t TTL)
{
	return Variables::TTL = Memory::set_TTL(TTL);
}

bool Serial_Driver::set_enabled(bool enabled)
{
	return Variables::ENABLED = Memory::set_enabled(enabled);
}