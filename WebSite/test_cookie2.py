#!/usr/bin/env python3

import os
import sys

cookies = {k: v for k, v in os.environ.items() if k.startswith('HTTP_COOKIE_')}

# Read input from stdin
a = ""
if os.environ.get('CONTENT_LENGTH'):
    content_length = int(os.environ['CONTENT_LENGTH'])
    a = sys.stdin.read(content_length)

a += input()

# Collect all output to calculate content length
output_lines = []
output_lines.append("I am the Python script and I received this as an input :")
output_lines.append(a)
for i in range(10):
    output_lines.append("I am the Python script and this is my " + str(i) + "th line of output")
output_lines.append("Found cookies: " + str(cookies))

# Combine all output
response_body = "\r\n".join(output_lines)

# Print headers first
print("Content-Type: text/plain")
print("Set-Cookie: cgi_test_cookie=worked")
print(f"Content-Length: {len(response_body)}")
print()  # Empty line to separate headers from body

# Print the response body
print(response_body)