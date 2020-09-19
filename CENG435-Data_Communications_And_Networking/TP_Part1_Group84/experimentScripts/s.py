#!/usr/bin/env python3
import time
from socket import *

def source(ip, port):
        c = socket(AF_INET, SOCK_DGRAM)
        i = 0
        while(i<5000):
                flag = 1
                try:
                        c.sendto(str(time.time()).encode(), (ip, port))
                except:
                        flag = 0
                if(flag):
                        i = i + 1

        i = 0;
        while(i<100):
                flag = 1
                try:
                        c.sendto(b'-', (ip, port))
                except:
                        flag = 0
                if(flag):
                        i = i + 1

        c.close()

source('10.10.3.2', 20462)
