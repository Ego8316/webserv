#!/usr/bin/env python3

exit(1)
input_str = input()
if (input_str.endswith("\r")):
	input_str = input_str[:-1]


output = f"I am the Python script and I received this as an input {input_str}\r\n\r\n"
for i in range(10):
	output += f"I am the Python script and this is my {i}th line of output\r\n"

print(f"Content-Length: {len(output) + 2}\r\n", end="\r\n")
print(output)