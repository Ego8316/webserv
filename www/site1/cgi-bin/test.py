#!/usr/bin/env python3

#print("Content-Length: " + str(622) + "\r\n", end="\r\n")

output = "I am the Python script and I received this as an input :\r\n\r\n"
for i in range(10):
	output += "I am the Python script and this is my " + str(i) + "th line of output\r\n"

print("Content-Length: " + str(len(output)) + "\r\n", end="\r\n")
print(output)