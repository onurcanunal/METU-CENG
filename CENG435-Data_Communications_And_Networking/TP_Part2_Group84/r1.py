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
			
	router1 = Thread(target=router, args=('10.10.4.2',60000, 61100))
	router2 = Thread(target=router, args=('10.10.4.2',60001, 61101))
	router3 = Thread(target=router, args=('10.10.4.2',60002, 61102))
	router4 = Thread(target=router, args=('10.10.4.2',60003, 61103))
	router5 = Thread(target=router, args=('10.10.4.2',60004, 61104))
	router6 = Thread(target=router, args=('10.10.4.2',60005, 61105))
	router7 = Thread(target=router, args=('10.10.4.2',60006, 61106))
	router8 = Thread(target=router, args=('10.10.4.2',60007, 61107))

	router1.start()
	router2.start()
	router3.start()
	router4.start()
	router5.start()
	router6.start()
	router7.start()
	router8.start()
