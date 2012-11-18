#!/usr/bin/env python
# -*- coding:utf-8 -*-

'''Client de test'''

import socket, sys

dico = {
	'i': 'I#ARM100,5,5,5,5,33,44,55#etagereBois,1,1,1,0,100,100,200\r\n',
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
}
print sorted(dico.keys())

sock = socket.socket()
sock.connect(('localhost', int(sys.argv[1])))
msg = ''
while msg not in ('q', 'quit', 'exit'):
	msg = raw_input('> ')
	msg = dico.get(msg, msg + '\r\n')
	sock.send(msg)
