#!/usr/bin/env python
# -*- coding:utf-8 -*-

from tcpclient import TCPClient
from infostests import *  # fichier de constantes
import time

class Client(TCPClient):

    def __init__(self, **kwargs):
        super(Client, self).__init__(**kwargs)
        self.log = []

    def on_connect(self):
        print '\t<CONNECT>'

    def on_disconnect(self, reason):
        print '\t<DISCONNECT "{0}">'.format(repr(reason))

    def on_data(self, data):
        self.log.append('<DATA "{}">'.format(repr(data)))


if __name__ == '__main__':
    for test in JEUX_DE_TESTS:
        print '==== LANCEMENT DU TEST "{}" ===='.format(test['titre'])
        client = Client(ip='127.0.0.1', port=8761)
        client.start()
        time.sleep(0.1)
        for cmd in test['commandes']:
            client.sendline(cmd)
            print '\t>>> ENVOI DE {}'.format(repr(cmd))
        time.sleep(0.1)
        client.stop()
        time.sleep(0.1)
        print '  ### RECU ###'
        for rcv in client.log:
            print '\t<<< {}'.format(repr(rcv))
        retour = 'SUCCES' if client.log == test['retour'] else 'ECHEC'
        print '  ### TEST = {} ###'.format(retour)

