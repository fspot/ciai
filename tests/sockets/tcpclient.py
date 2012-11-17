#!/usr/bin/env python
# -*- coding:utf-8 -*-

import threading
import socket
from pdb import set_trace as rrr

class TCPClient(threading.Thread):

    def __init__(self, ip=None, port=None, **kwargs):
        self._sock = None
        self.ip = ip
        self.port = port
        self.continuer = False
        super(TCPClient, self).__init__()

    def run(self):  # start()
        if self.ip is None or self.port is None:
            return
        try:
            self._sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self._sock.connect((self.ip, self.port))
        except:
            self.stop()
            self.on_disconnect('connect')
            return
        self.on_connect()
        self.continuer = True

        while self.continuer:
            try:
                data = self._sock.recv(1024)
                self._on_data(data)
            except:
                self.stop()
                self.on_disconnect('recv')
                return

    def stop(self):
        if self._sock is not None:
            try:
                self.continuer = False
                self._sock.shutdown(socket.SHUT_RDWR)
                self._sock.close()
            except:
                pass

    def send(self, msg):
        if self._sock is not None:
            try:
                self._sock.send(msg)
            except:
                self.stop()
                self.on_disconnect('send')

    def sendline(self, msg):
        self.send(msg + '\r\n')

    def _on_data(self, data):
        if data == '':
            self.stop()
            self.on_disconnect('serv_end')  # serv has quit : data is void.
        else:
            self.on_data(data)

    # you must reimplement these 3 functions :
    def on_connect(self):
        pass

    def on_disconnect(self, reason):
        pass

    def on_data(self, data):
        pass