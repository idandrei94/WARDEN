#pragma once

#include <Arduino.h>

class Serial_Driver
{
	/*
	Commands:
	WRD+DBGx! - set debug ON/OFF with 1/0
	WRD+IDxxxx! - set the ID of the unit
	WRD+BIDxxxx! - set the broadcast ID
	WRD+EIDxxxx! - set the ID of the end point
	WRD+LSTxxx! - set the list size for the message queues
	WRD+NIDx! - set the neighbour id of a slave unit
	WRD+GCHxx! - set group channel  (01 -> 10)
	WRD+BCHxx! - set bridge channel (11 -> 20) of a slave unit
	WRD+TTLxx! - set the TTL of the messages sent by this unit
	WRD+STATUSx! - enable/disable the unit
	WRD+PNGxxxx! - pings the targeted beacon ID
	*/

public:
	Serial_Driver();
	uint8_t read();
	bool get_id(uint16_t &id);

private:
	uint16_t ping_id = 0x00;
	bool ping_status = false;
	uint8_t count = 0;
	char buffer[12];

	void parse_serial_message();

	bool set_debug(bool value);
	uint16_t set_ID(uint16_t ID);
	uint16_t set_broadcast_ID(uint16_t ID);
	uint16_t set_endpoint_ID(uint16_t ID);
	uint8_t set_list_size(uint8_t size);
	uint8_t set_neighbour_ID(uint8_t ID);
	uint8_t set_group_channel(uint8_t channel);
	uint8_t set_bridge_channel(uint8_t channel);
	uint8_t set_TTL(uint8_t TTL);
	bool set_enabled(bool enabled);


	void reset_buffer();
};