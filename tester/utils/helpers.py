#! /usr/bin/env python3

import socket
from tester.config import TIMEOUT_SECONDS

def open_raw_connection():
    """ Opens a raw TCP connection for timeout tests. """
    s = socket.socket()
    s.settimeout(("localhost", 8081))
    return s
