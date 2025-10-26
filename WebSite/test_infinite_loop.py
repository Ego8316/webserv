#!/usr/bin/env python3

a = input()
print("Content-Length: " + str(len(a) + 638) + "\r\n", end="\r\n")
print("I am the Python script and I received this as an input :\r\n" + a, end="\r\n")
while (True):
	print("I am the Python script and this is a line of output", end="\r\n")