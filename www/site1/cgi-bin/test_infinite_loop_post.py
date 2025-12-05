#!/usr/bin/env python3

input_str = input()
if (input_str.endswith("\r")):
	input_str = input_str[:-1]


print(f"I am the Python script and I'll make you wait !, by the way thanks for {input_str}\r\n", end="\r\n")
while (True):
	pass