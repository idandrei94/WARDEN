def ping_reply(ID, timestamp):
	print("Ping reply with ID: " + hex(ID) + " and timestamp: " + str(timestamp) )

def receive_accident(ID, timestamp):
	print("Accident with ID: " + hex(ID) + " and timestamp: " + str(timestamp) )

def cancel_accident(ID, timestamp):
	print("Accident with ID: " + hex(ID) + " and timestamp: " + str(timestamp) + " canceled" ) 
