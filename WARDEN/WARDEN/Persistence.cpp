#include "Persistence.h"
#include "system_variables.h"
#include <EEPROM.h>

bool Memory::load_system_variables()
{
	// Default value of the EEPROM bytes is 255.
	// Reading 255 means the device hasn't been configured yet.
	Variables::ID = (EEPROM.read(Memory::ID_TOP_ADDRESS)<<8) + EEPROM.read(Memory::ID_BOT_ADDRESS);
	Variables::BROADCAST_ID = (EEPROM.read(Memory::BROADCAST_TOP_ADDRESS) << 8) + EEPROM.read(Memory::BROADCAST_BOT_ADDRESS);
	Variables::END_ID = (EEPROM.read(Memory::END_TOP_ADDRESS) << 8) + EEPROM.read(Memory::END_BOT_ADDRESS);
	Variables::TTL + EEPROM.read(Memory::TTL_ADDRESS);
	Variables::NEIGHBOUR_ID = EEPROM.read(Memory::NEIGHBOUR_ID_ADDRESS);
	Variables::LIST_SIZE = EEPROM.read(Memory::LIST_SIZE_ADDRESS);
	Variables::DEBUG = EEPROM.read(DEBUG_ADDRESS);
	Variables::BRIDGE_CHANNEL = EEPROM.read(Memory::BRIDGE_CHANNEL_ADDRESS);
	Variables::GROUP_CHANNEL = EEPROM.read(Memory::GROUP_CHANNEL_ADDRESS);
	Variables::ENABLED = EEPROM.read(Memory::ENABLED_ADDRESS) == 0x0 ? false : true;
	return true;
}

uint16_t Memory::set_ID(uint16_t ID)
{
	EEPROM.update(Memory::ID_TOP_ADDRESS, static_cast<uint8_t>(ID >> 8));
	EEPROM.update(Memory::ID_BOT_ADDRESS, static_cast<uint8_t>(ID));
	return ID;
}
uint16_t Memory::set_broadcast_ID(uint16_t ID)
{
	EEPROM.update(Memory::BROADCAST_TOP_ADDRESS, static_cast<uint8_t>(ID >> 8));
	EEPROM.update(Memory::BROADCAST_BOT_ADDRESS, static_cast<uint8_t>(ID));
	return ID;
}
uint16_t Memory::set_endpoint_ID(uint16_t ID)
{
	EEPROM.update(Memory::END_TOP_ADDRESS, static_cast<uint8_t>(ID >> 8));
	EEPROM.update(Memory::END_BOT_ADDRESS, static_cast<uint8_t>(ID));
	return ID;
}

uint8_t Memory::set_TTL(uint8_t TTL)
{
	EEPROM.update(Memory::TTL_ADDRESS, TTL);
	return TTL;
}

uint8_t Memory::set_list_size(uint8_t size)
{
	EEPROM.update(Memory::LIST_SIZE_ADDRESS, size);
	return size;
}

uint8_t Memory::set_neighbour_ID(uint8_t ID)
{
	EEPROM.update(Memory::NEIGHBOUR_ID_ADDRESS, ID);
	return ID;
}

bool Memory::set_debug(bool debug)
{
	EEPROM.update(Memory::DEBUG_ADDRESS, debug ? 1 : 0);
	return debug;
}

uint8_t Memory::set_group_channel(uint8_t channel)
{
	EEPROM.update(Memory::GROUP_CHANNEL_ADDRESS, channel);
	return channel;
}

uint8_t Memory::set_bridge_channel(uint8_t bridge)
{
	EEPROM.update(Memory::BRIDGE_CHANNEL_ADDRESS, bridge);
	return bridge;
}

bool Memory::set_enabled(bool enabled)
{
	EEPROM.update(Memory::ENABLED_ADDRESS, enabled ? 1 : 0);
	return enabled;
}