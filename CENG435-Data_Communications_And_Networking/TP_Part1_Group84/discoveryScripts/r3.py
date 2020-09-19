#!/usr/bin/env python3
import time
from socket import *
from threading import Thread

def client(ip, port, link):
    c = socket(AF_INET, SOCK_DGRAM)
    diff = 0
    i = 0
    while(i<1000):
        flag = 1
        beg = 0
        end = 0
        try:
            beg = time.time()
            c.sendto(b'rtt', (ip, port))
            result = c.recv(1000)
            end = time.time()
            i = i + 1
        except:
            flag = 0

        if(flag):
            diff += end - beg

    diff /= i;
    w = open("link_costs.txt", "a")
    w.write(link + " " diff + "\n")
    w.close()
    c.close()

client1 = Thread(target = client, args=('10.10.3.1', 20462, 's-r3:'))
client1.start()
client2 = Thread(target = client, args=('10.10.6.1', 20481, 'r2-r3:'))
client2.start()
client3 = Thread(target = client, args=('10.10.7.1', 20472, 'd-r3:'))
client3.start()
