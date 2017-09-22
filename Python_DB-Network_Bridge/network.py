import database
import time
import serial

ser = serial.Serial(port='/dev/ttyACM0',baudrate=9600, dsrdtr=True,bytesize=    serial.EIGHTBITS, timeout=1)

type_conv = {
b'\x01':"CALL_NEIGHBOURS",\
b'\x02':"GIVE_MASTER",\
b'\x03':"CONFIRM_GIVE_MASTER",\
b'\x04':"CONFIRM_MESSAGE",\
b'\x05':"REQ_PING",\
b'\x06':"CONFIRM_PING",\
b'\x07':"ACCIDENT",\
b'\x08':"CANCEL_ACCIDENT",\
b'\x09':"ERROR_MESSAGE",\
b'\x0a':"CALL_NEIGHBOURS_REPLY",\
"CALL_NEIGHBOURS":b'\x01',\
"GIVE_MASTER":b'\x02',\
"CONFIRM_GIVE_MASTER":b'\x03',\
"CONFIRM_MESSAGE":b'\x04',\
"REQ_PING":b'\x05',\
"CONFRIM_PING":b'\x06',\
"ACCIDENT":b'\x07',\
"CANCEL_ACCIDENT":b'\x08',\
"ERROR_MESSAGE":b'\x09',\
"CALL_NEIGHBOURS_REPLY":b'\x0a'
}

def to_byte(value):
	return (value%256).to_bytes(1, byteorder='big')

def bytes_to_word(left, right):
	return left * 256 + right

def network_handler():
	msg = ser.read(64)
	handle_message(msg)

def handle_message(message): 
	try:
		msg_type = type_conv[to_byte(message[0])]
		if msg_type == "CONFIRM_PING":
			ID = bytes_to_word(message[5], message[6])
			timestamp = int(time.time())
			database.ping_reply(ID, timestamp)
		elif msg_type == "ACCIDENT":
			ID = bytes_to_word(message[5], message[6]) 
			timestamp = int(time.time())
			database.receive_accident(ID, timestamp)
		elif msg_type == "CANCEL_ACCIDENT":
			ID = bytes_to_word(message[5], message[6]) 
			timestamp = int(time.time())
			database.cancel_accident(ID, timestamp)
	except KeyError:
		print("Invalid message type")

def send_ping(destination):
	string = "WRD+P" + str(destination)
	print("Sending string>>> " + string)
	ser.write(str.encode(string))
