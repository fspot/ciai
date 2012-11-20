#!/usr/bin/env python
# -*- coding:utf-8 -*-

from tcpclient import TCPClient
import time

dico = {
    'i2': 'I#A,1,1,5,1,1,1,1\r\n',
    'i3': 'I#A,3,3,3,1,1,1,1#B,1,1,5,1,1,1,1#C,1,1,5,1,1,1,1\r\n',
    'i': 'I#ARM100,2,2,2,10,33,44,55#etagereBois,2,2,2,10,100,100,200\r\n',
    'is': 'I#A,1,1,1,1,1,1,1\r\n',
    'il': 'I' + 63*'#A,1,1,1,1,1,1,1' + '\r\n',
    'itl': 'I' + 65*'#A,1,1,1,1,1,1,1' + '\r\n',
    'ib': 'I#A,1,1,1,1,1,1,1#B,1,1,1,1,1,1\r\n',
    'ib2': 'I#A,1,1,1,1,1,1,1#\r\n',
    'ib3': 'I#A_O,1,1,1,1,1,1,1\r\n',
    'ib4': 'I#A,1,1,1,E,1,1,1\r\n',
    'ib5': 'I#A,1,1,1,,1,1,1\r\n',
    'r0': 'R#0\r\n',
    'r1': 'R#1\r\n',
    'r2': 'R#2\r\n',
    'r3': 'R#3\r\n',
    'rb': 'R#5\r\n',
    'rb2': 'R#\r\n',
    'rb3': 'R#LOL\r\n',
    'rtl': 'R' + 'A'*1500 + '\r\n',
    'c': 'C#A,10#B,5\r\n',
    'c1a': 'C#A,1\r\n',
    'cb': 'C#A,1000\r\n',
}

class Client(TCPClient):

    def __init__(self, **kwargs):
        super(Client, self).__init__(**kwargs)

    def on_connect(self):
        print '\n<CONNECT>'

    def on_disconnect(self, reason):
        print '\n<DISCONNECT "{0}">'.format(repr(reason))

    def on_data(self, data):
        print '\n<RECEIVED "{}">'.format(repr(data))


if __name__ == '__main__':
    print sorted(dico.keys())
    client = Client(ip='127.0.0.1', port=8761)
    client.start()
    time.sleep(0.1)

    msg = ''
    while msg not in ('q', 'quit', 'exit'):
        msg = raw_input('> ')
        msg = dico.get(msg, msg + '\r\n')
        client.send(msg)

    client.stop()


