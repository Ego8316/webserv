#!/usr/bin/env python3

print("Content-Length: " + str(622) + "\r\n", end="\r\n")
print("I am the Python script and I received this as an input :\r\n", end="\r\n")
for i in range(10):
	print("I am the Python script and this is my " + str(i) + "th line of output", end="\r\n")

