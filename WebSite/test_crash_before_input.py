#!/usr/bin/env python3

import os

cookies = {k: v for k, v in os.environ.items() if k.startswith('HTTP_COOKIE_')}
exit(1)
a = input()

a += input()
print("Content-Type: text/plain", end="\r\n")
print("Set-Cookie: cgi_test_cookie=worked", end="\r\n")
print("I am the Python script and I received this as an input :", end="\r\n")
print(a)
print("Content-Length: " + str(len(a) + 60 ) + "\r\n", end="\r\n")
for i in range(10):
	print("I am the Python script and this is my " + str(i) + "th line of output", end="\r\n")

print("Found cookies:", cookies)
