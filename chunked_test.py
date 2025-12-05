#!/usr/bin/env python3

import requests
import time
import random
import logging
import http.client

http.client.HTTPConnection.debuglevel = 1

logging.basicConfig(level=logging.DEBUG)
requests_log = logging.getLogger("requests.packages.urllib3")
requests_log.setLevel(logging.DEBUG)
requests_log.propagate = True

def print_full_response(response):
	url = response.url
	try:
		protocol = "HTTP/1.1" 
		status_code = response.status_code
		reason = response.reason 
		
		status_line = f"{protocol} {status_code} {reason}"

		headers = ""
		for key, value in response.headers.items():
			headers += f"{key}: {value}\r\n"

		body = response.text
		
		full_response_message = (status_line + "\r\n" + headers + "\r\n" + body)

		print(f"\n+++++ Response for requested {url} +++++")
		print(full_response_message)
		print("\n" + "-"*50)

		print("\n+++++ FIELDS +++++")
		print(f"Status Code: {status_code}")
		print(f"Reason: {reason}")
		print(f"Content Length (based on header): {response.headers.get('Content-Length', 'N/A')}")
		print(f"Cookies Set by Server: {response.cookies.items()}")
		print(f"Request Duration: {response.elapsed.total_seconds():.3f} seconds")
	except Exception as e:
		print(f"An error occurred during response parsing: {e}")

lorem="Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus sit amet sollicitudin libero. Quisque pellentesque pellentesque porttitor. Phasellus a diam nulla. Nam enim ligula, aliquam eget consectetur sed, eleifend eget magna. Fusce lobortis libero quis metus feugiat, nec dapibus arcu congue. Donec molestie mi sed ante venenatis, id eleifend lorem vulputate. Aliquam erat volutpat. Pellentesque eget fermentum diam. Pellentesque a elit arcu. Nulla rhoncus euismod risus, et efficitur magna feugiat vitae. Donec eleifend at ex id tincidunt. Maecenas a enim vulputate, interdum neque in, suscipit odio. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Phasellus a augue lobortis, sollicitudin diam in, molestie tellus. In eros enim, faucibus a tellus et, malesuada scelerisque libero. Vestibulum vulputate, nunc in bibendum vestibulum, turpis enim aliquam dui, ac pellentesque nibh massa id leo. Nulla facilisi. Nam ut tellus nisl. Nulla vitae dui ac turpis pharetra lobortis. Curabitur vel fringilla sem. Cras sodales metus erat, vel posuere urna lacinia id. Morbi placerat sollicitudin mattis. Pellentesque semper in magna non aliquam. Etiam sagittis tellus quis urna condimentum, non tristique lacus placerat. Praesent viverra euismod enim, quis aliquet risus. Proin ac volutpat lorem, vitae condimentum justo. Etiam aliquet tempor lectus non facilisis. Integer varius, orci a ultrices consectetur, neque leo lobortis nisi, in semper lacus arcu non quam. Nunc non mauris at dolor commodo maximus eget non neque. Ut massa ante, tempor at ultrices et, viverra non diam. Suspendisse potenti. Sed eleifend finibus sollicitudin. Vestibulum sapien arcu, commodo eu dictum a, egestas non tortor. Maecenas vulputate eros ut fringilla cursus. Aliquam urna libero, faucibus non ipsum et, varius blandit libero. Donec vitae mauris eget ex luctus ultricies eget eu erat. Pellentesque ipsum mi, lacinia eu magna ac, molestie interdum nibh. Morbi at magna fringilla leo."	
random.seed(42)
def chunkstr():
	i = 0
	chunked = ""
	while i < len(lorem):
		size = random.randint(0, 16)
		if (i+size >= len(lorem)):
			size = len(lorem) - i
		chunked += hex(size) + "\r\n" + lorem[i:i+size] + "\r\n"
		i += size
	return (chunked)

url = 'http://127.0.0.1:8080/cgi-bin/test_post.py'

chunked_lorem = chunkstr()
print(chunked_lorem)
exit(0)
response = requests.post(url, data=chunked_lorem)
print_full_response(response)
