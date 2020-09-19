from socket import *
import hashlib
import threading
from threading import Thread
import time

if __name__ == "__main__":

	def router(ip, port1, port2):
                send_sock = socket(AF_INET, SOCK_DGRAM) #creating socket with UDP -- communication with destination
                receive_sock = socket(AF_INET, SOCK_DGRAM) #creating socket with UDP -- communication with source
                receive_sock.bind(('', port1)) #listening from the specified port
                send_sock.settimeout(0.1) #it's for retransmission of a packet when timeout

                while 1: #this part is for close routers. this packet is dummy packets which indicates communication is over
                        req_source, source = receive_sock.recvfrom(1000)
                        if(req_source == '-'):
                                break
                        while 1:
                                send_sock.sendto(req_source, (ip, port2))
                                try:
                                        res_destination = send_sock.recv(1000)
                                except timeout:
                                        continue
                                break
                        receive_sock.sendto(res_destination, source)

		receive_sock.close()

	#creation of threads and starting of them
			
	router1 = Thread(target=router, args=('10.10.5.2',60200, 61000))
	router2 = Thread(target=router, args=('10.10.5.2',60201, 61001))
	router3 = Thread(target=router, args=('10.10.5.2',60202, 61002))
	router4 = Thread(target=router, args=('10.10.5.2',60203, 61003))
	router5 = Thread(target=router, args=('10.10.5.2',60204, 61004))
	router6 = Thread(target=router, args=('10.10.5.2',60205, 61005))
	router7 = Thread(target=router, args=('10.10.5.2',60206, 61006))
	router8 = Thread(target=router, args=('10.10.5.2',60207, 61007))

	router1.start()
	router2.start()
	router3.start()
	router4.start()
	router5.start()
	router6.start()
	router7.start()
	router8.start()
