#pragma once
#include <Arduino.h>
namespace Memory
{
	const uint8_t ENABLED_ADDRESS = 0x00;
	const uint8_t ID_TOP_ADDRESS = 0x01;
	const uint8_t ID_BOT_ADDRESS = 0x02;
	const uint8_t BROADCAST_TOP_ADDRESS = 0x03;
	const uint8_t BROADCAST_BOT_ADDRESS = 0x04;
	const uint8_t END_TOP_ADDRESS = 0x05;
	const uint8_t END_BOT_ADDRESS = 0x06;
	const uint8_t TTL_ADDRESS = 0x07;
	const uint8_t LIST_SIZE_ADDRESS = 0x08;
	const uint8_t NEIGHBOUR_ID_ADDRESS = 0x09;
	const uint8_t DEBUG_ADDRESS = 0x0A;
	const uint8_t BRIDGE_CHANNEL_ADDRESS = 0x0B;
	const uint8_t GROUP_CHANNEL_ADDRESS = 0x0C;

	bool load_system_variables();

	uint16_t set_ID(uint16_t ID);
	uint16_t set_broadcast_ID(uint16_t ID);
	uint16_t set_endpoint_ID(uint16_t ID);
	uint8_t set_TTL(uint8_t TTL);
	uint8_t set_list_size(uint8_t size);
	uint8_t set_neighbour_ID(uint8_t ID);
	bool set_debug(bool debug);
	uint8_t set_group_channel(uint8_t channel);
	uint8_t set_bridge_channel(uint8_t bridge);
	bool set_enabled(bool enables);
}