from socket import *
import hashlib
import threading
from threading import Thread
from sys import argv
import time
import os

if __name__ == "__main__":
	experimentNo = int(argv[1]) #indicating that whether the experiment 1 or experiment 2

	if(experimentNo == 1): #if it is experiment 1, then do this block 
		window_size = 8
		rcvbase = 0 #it indicates that the smallest number of unreceived packet
		rcvbase_lock = threading.Lock() #we need to lock mechanism since rcvbase is used and can be changed by all threads.

		messageList = [""] * 6250 #we hold message contents of each packet in this list. initially, all entries are empty.

		def destination(port):
			global rcvbase #we used rcvbase as global
			global messageList #we used messageList as global

			receive_sock = socket(AF_INET, SOCK_DGRAM) #creating socket with UDP
			receive_sock.bind(('', port)) #listening from the specified port
			receive_sock.settimeout(5) #it's for terminating threads, used in try-except block below

			while 1:
				try:
					packet, router = receive_sock.recvfrom(1000) #waiting receiving packet from router
				except timeout:
					with rcvbase_lock:
						temp = rcvbase

					if(temp==6250): #if all packets are received successfully, then exiting the while loop
						break

					continue
				
				message_checksum = packet[:32] #first 32 bytes are checksum
				seq = int(packet[32:36]) #sequence number indicates the packet number of the packet
				message = packet[32:] #message content
				calculated_checksum = hashlib.md5(message).hexdigest() #checksum calculation of seq + message content
				message = message[4:] #excluding seq from message content
				if(calculated_checksum != message_checksum): #the packet is corrupted!
					ack = "-001"
				else:
					with rcvbase_lock:
						if(rcvbase < 6250):
							if(seq-rcvbase < window_size): #sequence number have to be in window
								messageList[seq] = message
								if(seq == rcvbase):
									newBase = seq + 1
									while(newBase < 6250 and messageList[newBase] != ""):
										newBase = newBase + 1
									rcvbase = newBase
								ack = packet[32:36]
							else:
								ack = "-001"

				checksum = hashlib.md5(ack + message).hexdigest()
				receive_sock.sendto((checksum+ack+message), router)

			receive_sock.close()
				
		#creation of threads and starting of them

		destination1 = Thread(target=destination, args=(61000,))
		destination2 = Thread(target=destination, args=(61001,))
		destination3 = Thread(target=destination, args=(61002,))
		destination4 = Thread(target=destination, args=(61003,))
		destination5 = Thread(target=destination, args=(61004,))
		destination6 = Thread(target=destination, args=(61005,))
		destination7 = Thread(target=destination, args=(61006,))
		destination8 = Thread(target=destination, args=(61007,))

		destination1.start()
		destination2.start()
		destination3.start()
		destination4.start()
		destination5.start()
		destination6.start()
		destination7.start()
		destination8.start()

		while(rcvbase < 6250): #until all packets are received, wait to write them to output.txt
			continue

		endTime = time.time()
		print(endTime)

		outputFile = open("output1.txt", "w")

		for i in range(6250):
			outputFile.write(messageList[i])

		outputFile.close()

	elif(experimentNo == 2): #if it is experiment 2, then do this block 
		window_size = 8
		r1_rcvbase = 0 #it indicates that the smallest number of unreceived packet for route 1
		r2_rcvbase = 6249 #it indicates that the biggest number of unreceived packet for route 2
		r1_rcvbase_lock = threading.Lock() #we need to lock mechanism since it is used and can be changed by all threads.
		r2_rcvbase_lock = threading.Lock() #we need to lock mechanism since it is used and can be changed by all threads.

		messageList = [""] * 6250 #we hold message contents of each packet in this list. initially, all entries are empty.

		def destination1(port): #this function is for threads sending packets beginning of the file
			global r1_rcvbase #we used rcvbase as global
			global r2_rcvbase #we used messageList as global
			global messageList

			receive_sock = socket(AF_INET, SOCK_DGRAM) #creating socket with UDP
			receive_sock.bind(('', port)) #listening from the specified port
			receive_sock.settimeout(5) #it's for terminating threads, used in try-except block below

			while 1:
				try:
					packet, router = receive_sock.recvfrom(1000) #waiting receiving packet from router
				except timeout:
					if(r1_rcvbase > r2_rcvbase): #if all packets are received successfully, then exiting the while loop
						break
					continue
				
				message_checksum = packet[:32] #first 32 bytes are checksum
				seq = int(packet[32:36]) #sequence number indicates the packet number of the packet
				message = packet[32:] #message content
				calculated_checksum = hashlib.md5(message).hexdigest() #checksum calculation of seq + message content
				message = message[4:] #excluding seq from message content
				if(calculated_checksum != message_checksum): #the packet is corrupted!
					ack = "-001"
				else:
					with r2_rcvbase_lock:
						temp_r2_rcvbase = r2_rcvbase
					with r1_rcvbase_lock:
						temp_r1_rcvbase = r1_rcvbase
					with r1_rcvbase_lock:
						if(seq-r1_rcvbase < window_size): #sequence number have to be in window
							messageList[seq] = message
							if(seq == r1_rcvbase):
								newBase = seq + 1
								while(newBase <= temp_r2_rcvbase and messageList[newBase] != ""):
									newBase = newBase + 1
								r1_rcvbase = newBase
							ack = packet[32:36]
						else:
							ack = "-001"

				checksum = hashlib.md5(ack + message).hexdigest()
				receive_sock.sendto((checksum+ack+message), router)

			receive_sock.close()

		def destination2(port): #this function is for threads sending packets end of the file, the same approach with destination1
			global r1_rcvbase
			global r2_rcvbase
			global messageList

			receive_sock = socket(AF_INET, SOCK_DGRAM)
			receive_sock.bind(('', port))
			receive_sock.settimeout(5)

			while 1:
				try:
					packet, router = receive_sock.recvfrom(1000)
				except timeout:
					if(r1_rcvbase > r2_rcvbase):
						break
					continue
				
				message_checksum = packet[:32]
				seq = int(packet[32:36])
				message = packet[32:]
				calculated_checksum = hashlib.md5(message).hexdigest()
				message = message[4:]
				if(calculated_checksum != message_checksum):
					ack = "-001"
				else:
					with r2_rcvbase_lock:
						temp_r2_rcvbase = r2_rcvbase
					with r1_rcvbase_lock:
						temp_r1_rcvbase = r1_rcvbase
					with r2_rcvbase_lock:
						if(r2_rcvbase-seq < window_size):
							messageList[seq] = message
							if(seq == r2_rcvbase):
								newBase = seq - 1
								while(newBase >= temp_r1_rcvbase and messageList[newBase] != ""):
									newBase = newBase - 1
								r2_rcvbase = newBase
							ack = packet[32:36]
						else:
							ack = "-001"

				checksum = hashlib.md5(ack + message).hexdigest()
				receive_sock.sendto((checksum+ack+message), router)

			receive_sock.close()

		#creation of threads and starting of them				

		r1_destination1 = Thread(target=destination1, args=(61100,))
		r1_destination2 = Thread(target=destination1, args=(61101,))
		r1_destination3 = Thread(target=destination1, args=(61102,))
		r1_destination4 = Thread(target=destination1, args=(61103,))
		r1_destination5 = Thread(target=destination1, args=(61104,))
		r1_destination6 = Thread(target=destination1, args=(61105,))
		r1_destination7 = Thread(target=destination1, args=(61106,))
		r1_destination8 = Thread(target=destination1, args=(61107,))

		r1_destination1.start()
		r1_destination2.start()
		r1_destination3.start()
		r1_destination4.start()
		r1_destination5.start()
		r1_destination6.start()
		r1_destination7.start()
		r1_destination8.start()

		r2_destination1 = Thread(target=destination2, args=(61000,))
		r2_destination2 = Thread(target=destination2, args=(61001,))
		r2_destination3 = Thread(target=destination2, args=(61002,))
		r2_destination4 = Thread(target=destination2, args=(61003,))
		r2_destination5 = Thread(target=destination2, args=(61004,))
		r2_destination6 = Thread(target=destination2, args=(61005,))
		r2_destination7 = Thread(target=destination2, args=(61006,))
		r2_destination8 = Thread(target=destination2, args=(61007,))

		r2_destination1.start()
		r2_destination2.start()
		r2_destination3.start()
		r2_destination4.start()
		r2_destination5.start()
		r2_destination6.start()
		r2_destination7.start()
		r2_destination8.start()

		while(r1_rcvbase - 1 < r2_rcvbase): #until all packets are received, wait to write them to output.txt
			continue

		endTime = time.time()
		print(endTime)

		outputFile = open("output2.txt", "w")

		for i in range(6250):
			outputFile.write(messageList[i])

		outputFile.close()
		os._exit(1)      #terminate script
