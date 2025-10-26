#!/usr/bin/env python3

a = input()
print("Content-Length: " + str(len(a) + 618) + "\r\n")
print("I am the Python script and I received this as an input :\r\n" + a)
for i in range(10):
	print("I am the Python script and this is my " + str(i) + "th line of output")
#while (True):
#	continue
