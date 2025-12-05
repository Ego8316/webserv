#!/usr/bin/env python3

output = "I am the Python script and I received this as an input :\r\n\r\n"
for i in range(10):
	output += f"I am the Python script and this is my {i}th line of output\r\n"

print("Content-Length: " + str(len(output) + 2) + "\r\n", end="\r\n")
print(output, end="\r\n")