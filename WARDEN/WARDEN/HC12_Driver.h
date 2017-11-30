#pragma once
#include <Arduino.h>
#include <SoftwareSerial.h>

class HC12_Driver {
public:
	enum class HC12_Modes {
		GROUP,
		BRIDGE
	};
	/** Performs the HC12 init
	*	@param mode			Initialize the group or the group bridge module
	*/
	HC12_Driver(HC12_Driver::HC12_Modes mode, int timeout = 1000);

	/** Sends data via HC12
	 * @param data			The array of bytes to send
	 * @param bytesToSend	How many bytes to send
	 */
	void send_message(uint8_t* data, uint8_t bytesToSend);

	/** Attempt to receive a message from HC12
	 * @param buffer		The buffer where the message will be copied
	 * @pre					The buffer must have at least 64 bytes of memory
	 * @return				Message length, 0 if a full message was not received
	 */
	uint8_t get_message(uint8_t buffer[], uint8_t length);

	/** Execute an AT command on the HC12 module
	 * @param command	The string containing the command
	 * @pre				The string must be a valid AT command
	 * @pre				0 < length <= 64
	 */
	void send_command(String command);

	/** Set the timeout for receiving a message
	*	@param _timeout	The new timeout in milliseconds
	*/
	void set_timeout(int _timeout);

private:
	const static byte HC12RxdPin = 11;              // RXD pin
	const static byte HC12TxdPin = 10;              // TXD pin
	const static byte HC12SetPin = 9;               // SET pin

	const static byte HC12BridgeRxdPin = 7;              // RXD pin
	const static byte HC12BridgeTxdPin = 6;              // TXD pin
	const static byte HC12BridgeSetPin = 5;              // SET pin

	SoftwareSerial HC12;    // Software Serial Port for the HC12

	uint8_t read_buffer[64];                           // Read buffer for HC12
	int timeout;
	HC12_Modes mode;


	/** Initialize the HC12 module
	 */
	void init_HC12(HC12_Modes mode);
};