#!/usr/bin/python3
import network
import database

network.ser.flush()
i = 0;
while True:
	network.network_handler()
	network.send_ping(i)
	i = i+1
	print()

