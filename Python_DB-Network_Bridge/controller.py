#!/usr/bin/python3
import network
import database

network.ser.flush()
while True:
	network.network_handler()
	database.database_handler()	
	print()
