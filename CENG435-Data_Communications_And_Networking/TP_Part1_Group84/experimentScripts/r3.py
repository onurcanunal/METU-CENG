#!/usr/bin/env python3
import time
from socket import *

def router(ip, port1, port2):
        r = socket(AF_INET, SOCK_DGRAM)
        r.bind(('', port1))
        while(1):
                req = r.recv(1000)
                r.sendto(req, (ip, port2))
                if(req.decode() == '-'):
                        break

        i = 0;
        while(i<100):
                flag = 1
                try:
                        r.sendto(b'-', (ip, port2))
                except:
                        flag = 0
                if(flag):
                        i = i + 1

        r.close()

router('10.10.7.1', 20462, 20472)