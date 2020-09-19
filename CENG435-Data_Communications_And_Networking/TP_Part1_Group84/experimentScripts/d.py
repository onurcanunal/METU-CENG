#!/usr/bin/env python3
import time
from socket import *
import statistics

def destination(port):
        s = socket(AF_INET, SOCK_DGRAM)
        s.bind(('', port))
        result = []
        while(1):
                req = s.recv(1000)
                arrival = time.time()
                try:
                        result.append(arrival - float(req.decode()))
                except:
                        break
        w = open("delays.txt", "a")
        avg = statistics.mean(result)
        std = statistics.stdev(result, avg)
        print("Number of packets: ", len(result))
        print("Avg: ", avg)
        print("Standart deviation: ", std)
        w.write("Avg: " + str(avg))
        w.close()
        s.close()

destination(20472)