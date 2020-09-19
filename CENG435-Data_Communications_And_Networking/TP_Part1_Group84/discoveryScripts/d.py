#!/usr/bin/env python3
import time
from socket import *
from threading import Thread

def server(port):
	s = socket(AF_INET, SOCK_DGRAM)
	s.bind(('', port))
	for i in range(1000):
		req, peer = s.recvfrom(10000)
		s.sendto(req, peer)
	s.close()

server1 = Thread(target = server, args=(20470,))
server1.start()
server2 = Thread(target = server, args=(20471,))
server2.start()
server3 = Thread(target = server, args=(20472,))
server3.start()