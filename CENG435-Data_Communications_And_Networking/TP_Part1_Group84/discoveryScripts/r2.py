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

def server(port):
    s = socket(AF_INET, SOCK_DGRAM)
    s.bind(('', port))
    for i in range(1000):
        req, peer = s.recvfrom(10000)
        s.sendto(req, peer)
    s.close()

client1 = Thread(target = client, args=('10.10.2.2', 20461, 's-r2:'))
client1.start()
client2 = Thread(target = client, args=('10.10.5.2', 20471, 'd-r2:'))
client2.start()
server1 = Thread(target = server, args=(20480,))
server1.start()
server2 = Thread(target = server, args=(20481,))
server2.start()