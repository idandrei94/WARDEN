#include "HC12_Driver.h"
#include "Message.h"

HC12_Driver::HC12_Driver(HC12_Driver::HC12_Modes mode, int timeout = 1000) : timeout(timeout), mode(mode),
HC12(
	mode==HC12_Driver::HC12_Modes::BRIDGE?HC12BridgeTxdPin:HC12TxdPin, 
	mode == HC12_Driver::HC12_Modes::BRIDGE ? HC12BridgeRxdPin : HC12RxdPin)
{
	init_HC12(mode);
}

void HC12_Driver::init_HC12(HC12_Driver::HC12_Modes mode) {
	switch (mode) {
	case HC12_Driver::HC12_Modes::GROUP:
	{
		if (Variables::DEBUG)
			Serial.println("Group HC12 initializing...");
		pinMode(HC12SetPin, OUTPUT);                // Output HIGH for normal mode, LOW for AT Command mode
		digitalWrite(HC12SetPin, HIGH);             // Normal mode
		delay(80);                                  // 80ms delay before operation (Datasheet)
		HC12.begin(9600);                           // Open HC12 Port
		String comm = "AT+C";
		if (Variables::GROUP_CHANNEL*5 < 100)
			comm += 0;
		if (Variables::GROUP_CHANNEL * 5 < 10)
			comm += 0;
		send_command(comm + Variables::GROUP_CHANNEL * 5);
		break;
	}
	case HC12_Driver::HC12_Modes::BRIDGE:
		if(Variables::DEBUG)
			Serial.println("Bridge HC12 initializing...");
		pinMode(HC12BridgeSetPin, OUTPUT);          // Output HIGH for normal mode, LOW for AT Command mode
		digitalWrite(HC12BridgeSetPin, HIGH);       // Normal mode
		delay(80);                                  // 80ms delay before operation (Datasheet)
		HC12.begin(9600);                           // Open HC12 Port
		String comm = "AT+C";
		if (Variables::BRIDGE_CHANNEL < 100)
			comm += 0;
		if (Variables::BRIDGE_CHANNEL < 10)
			comm += 0;
		send_command(comm + Variables::BRIDGE_CHANNEL);
		break;
	}
}

void HC12_Driver::send_message(uint8_t* data, uint8_t bytesToSend) {
	HC12.write(data, bytesToSend);
}

uint8_t HC12_Driver::get_message(uint8_t buffer[], uint8_t length) {
	HC12.listen();
	int index = 0;
	unsigned long timestamp = millis();
	while (true)
	{
		if (millis() - timestamp > timeout)
		{
			return index;
		}
		if (HC12.available())
		{
			timestamp = millis();
			read_buffer[index++] = HC12.read();
			if (index == length)
			{
				for (uint8_t i = 0; i < length; ++i)
					buffer[i] = read_buffer[i];
				return index;
			}
		}
	}
}

void HC12_Driver::send_command(String command) {
	uint8_t pin = mode == HC12_Driver::HC12_Modes::BRIDGE ? HC12BridgeSetPin : HC12SetPin;
	digitalWrite(pin, LOW);      // Enter Command mode
	delay(100);                         // Short delay as per datasheet
	HC12.print(command);                // Send command to the local HC12
	delay(500);                         // Wait for response
	digitalWrite(pin, HIGH);     // Back to normal mode
	delay(100);
	if (Variables::DEBUG)
	{
		String res = "";
		while (HC12.available())
		{
			res += (char)HC12.read();
		}
		Serial.println(res);
	}
}

void HC12_Driver::set_timeout(int _timeout)
{
	timeout = _timeout;
}