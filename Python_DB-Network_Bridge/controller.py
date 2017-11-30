#!/usr/bin/python3.5
import network
import database

network.ser.flush()
while True:
	network.network_handler()
	try:
		database.database_handler()	
	except:
		print("DB error")
	print()
