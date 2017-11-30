#pragma once
#include <Arduino.h>

namespace Variables {
	/**
	*	ID		The unique ID of the arduino, stored persistently
	*/
	extern uint16_t ID;
	extern uint16_t BROADCAST_ID;
	extern uint16_t END_ID;
	extern uint8_t TTL;
	extern uint8_t LIST_SIZE;
	extern uint8_t NEIGHBOUR_ID;
	extern bool DEBUG;
	extern uint8_t BRIDGE_CHANNEL;
	extern uint8_t GROUP_CHANNEL;
	extern bool ENABLED;
}