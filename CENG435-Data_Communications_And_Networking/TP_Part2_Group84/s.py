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
		with open("input1.txt") as f:
			text = f.read()

		acked_list = [False]*6250 #we hold ack information of each packet in this list. initially, all entries are False and it means that no acked is received.
		packet_no = 0 #it indicates that the smallest number of not sended packet
		smallest_unack_value = 0 #it indicates that the smallest unacked packet
		window_size = 8

		packet_no_lock = threading.Lock() #we need to lock mechanism since it is used and can be changed by all threads.
		smallest_unack_value_lock = threading.Lock() #we need to lock mechanism since it is used and can be changed by all threads.

		def source(ip, port):
			global packet_no #we used packet_no as global
			global acked_list #we used acked_list as global
			global smallest_unack_value #we used smallest_unack_value as global

			send_sock = socket(AF_INET, SOCK_DGRAM) #creating socket with UDP
			send_sock.settimeout(0.1) #it's for retransmission of packets, used in try-except block below
			
			size = 5000000
			segment_size = 800

			while 1:
				#lock
				with packet_no_lock:
					local_packet_no = packet_no
					packet_no = packet_no + 1
				#unlock
				if(local_packet_no >= 6250):
					i = 0;
					while(i<100): #this part is for close routers. this packet is dummy packets which indicates communication is over
							flag = 1
							try:
									send_sock.sendto(b'-', (ip, port))
							except:
									flag = 0
							if(flag):
									i = i + 1
					break #if all packets are successfully sent, then break

				while 1:
					if(local_packet_no - smallest_unack_value <= window_size-1): #sequence number have to be in window
						sequence_no = str(local_packet_no)
						message = text[local_packet_no*segment_size:(local_packet_no+1)*segment_size]
						
						checksum = hashlib.md5(sequence_no.zfill(4) + message).hexdigest()
						
						send_sock.sendto((checksum + sequence_no.zfill(4) + message), (ip, port))
						#zfill(4) is used in order to padding the string and obtain 4 bytes string every time

						try:
							message, address = send_sock.recvfrom(1000)
						except timeout:
							continue

						message_checksum = message[:32] #checksum 32 bytes
						ack = int(message[32:36])
						
						message_content = message[32:]
						
						calculated_checksum = hashlib.md5(message_content).hexdigest() #checksum calculation

						# the packet will be accepted if ack is the right value and checksum test is successfully passed
						if(ack != -1 and ack == local_packet_no and calculated_checksum == message_checksum):
							#lock
							with smallest_unack_value_lock:

								acked_list[ack] = True
								if(ack == smallest_unack_value):
									counter = ack + 1
									while (counter < 6250 and acked_list[counter]):
										counter = counter + 1
									smallest_unack_value = counter
							#unclock
							break
			send_sock.close()

		startTime = time.time()
		print(startTime)

		#creation of threads and starting of them

		source1 = Thread(target=source, args=('10.10.3.2',60000))
		source2 = Thread(target=source, args=('10.10.3.2',60001))
		source3 = Thread(target=source, args=('10.10.3.2',60002))
		source4 = Thread(target=source, args=('10.10.3.2',60003))
		source5 = Thread(target=source, args=('10.10.3.2',60004))
		source6 = Thread(target=source, args=('10.10.3.2',60005))
		source7 = Thread(target=source, args=('10.10.3.2',60006))
		source8 = Thread(target=source, args=('10.10.3.2',60007))

		source1.start()
		source2.start()
		source3.start()
		source4.start()
		source5.start()
		source6.start()
		source7.start()
		source8.start()

	elif(experimentNo == 2): #if it is experiment 2, then do this block 
		with open("input2.txt") as f:
			text = f.read()

		acked_list = [False]*6250 #we hold ack information of each packet in this list. initially, all entries are False and it means that no acked is received.
		r1_packet_no = 0 #it indicates that the smallest number of not sended packet (for route 1)
		r2_packet_no = 6249 #it indicates that the biggest number of not sended packet (for route 2)
		smallest_unack_value = 0 #it indicates that the smallest unacked packet (for route 1)
		biggest_unack_value = 6249 #it indicates that the biggest unacked packet (for route 2)
		window_size = 8

		r1_packet_no_lock = threading.Lock() #we need to lock mechanism since it is used and can be changed by all threads.
		r2_packet_no_lock = threading.Lock() #we need to lock mechanism since it is used and can be changed by all threads.
		smallest_unack_value_lock = threading.Lock() #we need to lock mechanism since it is used and can be changed by all threads.
		biggest_unack_value_lock = threading.Lock() #we need to lock mechanism since it is used and can be changed by all threads.

		def source1(ip, port): # it has similar approach with the previous source function except termination checks
			global r1_packet_no
			global acked_list
			global smallest_unack_value
			global biggest_unack_value

			send_sock = socket(AF_INET, SOCK_DGRAM)
			send_sock.settimeout(0.1)

			break_flag = False
			
			size = 5000000
			segment_size = 800

			while 1:
				timeout_counter = 0
				#lock
				with r1_packet_no_lock:
					local_packet_no = r1_packet_no
					r1_packet_no = r1_packet_no + 1
				#unlock
				if(False not in acked_list): #it indicates whether all ack values are successfully received
					i = 0;
					while(i<100):
							flag = 1
							try:
									send_sock.sendto(b'-', (ip, port))
							except:
									flag = 0
							if(flag):
									i = i + 1
					os._exit(1)

				while 1:
					if(local_packet_no - smallest_unack_value <= window_size-1):
						sequence_no = str(local_packet_no)
						message = text[local_packet_no*segment_size:(local_packet_no+1)*segment_size]
						
						checksum = hashlib.md5(sequence_no.zfill(4) + message).hexdigest()
						
						send_sock.sendto((checksum + sequence_no.zfill(4) + message), (ip, port))

						try:
							message, address = send_sock.recvfrom(1000)
						except timeout:
							continue

						timeout_counter = 0

						
						message_checksum = message[:32] #checksum 32 bytes
						ack = int(message[32:36])
						message_content = message[32:]
						
						calculated_checksum = hashlib.md5(message_content).hexdigest()

						if(ack != -1 and ack == local_packet_no and calculated_checksum == message_checksum):
							#lock
							with smallest_unack_value_lock:
								acked_list[ack] = True
								if(ack == smallest_unack_value):
									counter = ack + 1
									while (counter < 6250 and acked_list[counter]):
										counter = counter + 1
									smallest_unack_value = counter
							#unclock
							break
			send_sock.close()

		def source2(ip, port): # it has similar approach with the previous source function except termination checks
			global r2_packet_no
			global acked_list
			global smallest_unack_value
			global biggest_unack_value

			send_sock = socket(AF_INET, SOCK_DGRAM)
			send_sock.settimeout(0.1)

			break_flag = False

			size = 5000000
			segment_size = 800

			while 1:		
				timeout_counter = 0
				#lock
				with r2_packet_no_lock:
					local_packet_no = r2_packet_no
					r2_packet_no = r2_packet_no - 1
				#unlock
				if(False not in acked_list): #it indicates whether all ack values are successfully received
					i = 0;
					while(i<100):
							flag = 1
							try:
									send_sock.sendto(b'-', (ip, port))
							except:
									flag = 0
							if(flag):
									i = i + 1
					os._exit(1)

				while 1:			
					if(biggest_unack_value - local_packet_no <= window_size-1):
						sequence_no = str(local_packet_no)
						message = text[local_packet_no*segment_size:(local_packet_no+1)*segment_size]
						
						checksum = hashlib.md5(sequence_no.zfill(4) + message).hexdigest()
						
						send_sock.sendto((checksum + sequence_no.zfill(4) + message), (ip, port))

						try:
							message, address = send_sock.recvfrom(1000)
						except timeout:
							continue

						timeout_counter = 0

						message_checksum = message[:32] #checksum 32 bytes
						ack = int(message[32:36])
						message_content = message[32:]
						
						calculated_checksum = hashlib.md5(message_content).hexdigest()

						if(ack != -1 and ack == local_packet_no and calculated_checksum == message_checksum):
							#lock
							with biggest_unack_value_lock:
								acked_list[ack] = True
								if(ack == biggest_unack_value):
									counter = ack - 1
									while (counter > -1 and acked_list[counter]):
										counter = counter - 1
									biggest_unack_value = counter
							#unclock
							break
			send_sock.close()

		startTime = time.time()
		print(startTime)

		#creation of threads and starting of them

		source1_1 = Thread(target=source1, args=('10.10.1.2',60000))
		source1_2 = Thread(target=source1, args=('10.10.1.2',60001))
		source1_3 = Thread(target=source1, args=('10.10.1.2',60002))
		source1_4 = Thread(target=source1, args=('10.10.1.2',60003))
		source1_5 = Thread(target=source1, args=('10.10.1.2',60004))
		source1_6 = Thread(target=source1, args=('10.10.1.2',60005))
		source1_7 = Thread(target=source1, args=('10.10.1.2',60006))
		source1_8 = Thread(target=source1, args=('10.10.1.2',60007))

		source1_1.start()
		source1_2.start()
		source1_3.start()
		source1_4.start()
		source1_5.start()
		source1_6.start()
		source1_7.start()
		source1_8.start()

		source2_1 = Thread(target=source2, args=('10.10.2.1',60200))
		source2_2 = Thread(target=source2, args=('10.10.2.1',60201))
		source2_3 = Thread(target=source2, args=('10.10.2.1',60202))
		source2_4 = Thread(target=source2, args=('10.10.2.1',60203))
		source2_5 = Thread(target=source2, args=('10.10.2.1',60204))
		source2_6 = Thread(target=source2, args=('10.10.2.1',60205))
		source2_7 = Thread(target=source2, args=('10.10.2.1',60206))
		source2_8 = Thread(target=source2, args=('10.10.2.1',60207))

		source2_1.start()
		source2_2.start()
		source2_3.start()
		source2_4.start()
		source2_5.start()
		source2_6.start()
		source2_7.start()
		source2_8.start()

