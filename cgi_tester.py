import requests

RESET = '\033[0m'
RED = '\033[31m'
BLUE = '\033[0;34m'

url = "http://127.0.0.1:8080/"
lorem="Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus sit amet sollicitudin libero. Quisque pellentesque pellentesque porttitor. Phasellus a diam nulla. Nam enim ligula, aliquam eget consectetur sed, eleifend eget magna. Fusce lobortis libero quis metus feugiat, nec dapibus arcu congue. Donec molestie mi sed ante venenatis, id eleifend lorem vulputate. Aliquam erat volutpat. Pellentesque eget fermentum diam. Pellentesque a elit arcu. Nulla rhoncus euismod risus, et efficitur magna feugiat vitae. Donec eleifend at ex id tincidunt. Maecenas a enim vulputate, interdum neque in, suscipit odio. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Phasellus a augue lobortis, sollicitudin diam in, molestie tellus. In eros enim, faucibus a tellus et, malesuada scelerisque libero. Vestibulum vulputate, nunc in bibendum vestibulum, turpis enim aliquam dui, ac pellentesque nibh massa id leo. Nulla facilisi. Nam ut tellus nisl. Nulla vitae dui ac turpis pharetra lobortis. Curabitur vel fringilla sem. Cras sodales metus erat, vel posuere urna lacinia id. Morbi placerat sollicitudin mattis. Pellentesque semper in magna non aliquam. Etiam sagittis tellus quis urna condimentum, non tristique lacus placerat. Praesent viverra euismod enim, quis aliquet risus. Proin ac volutpat lorem, vitae condimentum justo. Etiam aliquet tempor lectus non facilisis. Integer varius, orci a ultrices consectetur, neque leo lobortis nisi, in semper lacus arcu non quam. Nunc non mauris at dolor commodo maximus eget non neque. Ut massa ante, tempor at ultrices et, viverra non diam. Suspendisse potenti. Sed eleifend finibus sollicitudin. Vestibulum sapien arcu, commodo eu dictum a, egestas non tortor. Maecenas vulputate eros ut fringilla cursus. Aliquam urna libero, faucibus non ipsum et, varius blandit libero. Donec vitae mauris eget ex luctus ultricies eget eu erat. Pellentesque ipsum mi, lacinia eu magna ac, molestie interdum nibh. Morbi at magna fringilla leo."	
minilorem="Lorem ipsum dolor sit amet\r\n"
chunked = "0x3\r\nLor\r\n0x8\r\nem ipsum\r\n0x7\r\n dolor \r\n0x7\r\nsit ame\r\n0x4\r\nt, c\r\n0x3\r\nons\r\n0x2\r\nec\r\n0xd\r\ntetur adipisc\r\n0x1\r\ni\r\n0x2\r\nng\r\n0x6\r\n elit.\r\n0x7\r\n Vivamu\r\n0x10\r\ns sit amet solli\r\n0x6\r\ncitudi\r\n0xd\r\nn libero. Qui\r\n0x7\r\nsque pe\r\n0xe\r\nllentesque pel\r\n0x8\r\nlentesqu\r\n0x0\r\n"

def get_full_http_response_from_object(response):
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


################# NO BODY TEST #############################

testf = "test_get.py"
print(f"{RED}" + testf + f"{RESET}")
response = requests.get(url + "cgi-bin/"+testf)
print(f"{BLUE}" + testf + " returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)
input("press enter to continue")
testf = "test_infinite_loop_get.py"
print(f"{RED}" + testf + f"{RESET}")
response = requests.get(url + "cgi-bin/"+testf)
print(f"{BLUE}" + testf + " returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")
testf = "test_cookies_get.py"
print(f"{RED}" + testf + f"{RESET}")
response = requests.get(url + "cgi-bin/"+testf)
print(f"{BLUE}" + testf + " returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")

################# MINILOREM TEST #############################
testf = "test_post.py"
print(f"{RED}" + testf + f" - minilorem{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=minilorem)
print(f"{BLUE}" + testf + " - minilorem returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")
testf = "test_infinite_loop_post.py"
print(f"{RED}" + testf + f" - minilorem{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=minilorem)
print(f"{BLUE}" + testf + " - minilorem returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")
testf = "test_cookies_post.py"
print(f"{RED}" + testf + f" - minilorem{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=minilorem)
print(f"{BLUE}" + testf + " - minilorem returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")
testf = "test_crash_after_input.py"
print(f"{RED}" + testf + f" - minilorem{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=minilorem)
print(f"{BLUE}" + testf + " - minilorem returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")
testf = "test_crash_before_input.py"
print(f"{RED}" + testf + f" - minilorem{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=minilorem)
print(f"{BLUE}" + testf + " - minilorem returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)


input("press enter to continue")
print(f"{RED}test_noread_input.py - minilorem{RESET}")
testf = "test_noread_input.py"
print(f"{RED}" + testf + f" - minilorem{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=minilorem)
print(f"{BLUE}" + testf + " - minilorem returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)


input("press enter to continue")
################# LOREM TEST #############################

testf = "test_post.py"
print(f"{RED}" + testf + f" - lorem{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=lorem)
print(f"{BLUE}" + testf + " - lorem returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")
testf = "test_infinite_loop_post.py"
print(f"{RED}" + testf + f" - lorem{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=lorem)
print(f"{BLUE}" + testf + " - lorem returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")
testf = "test_cookies_post.py"
print(f"{RED}" + testf + f" - lorem{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=lorem)
print(f"{BLUE}" + testf + " - lorem returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")
testf = "test_crash_after_input.py"
print(f"{RED}" + testf + f" - lorem{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=lorem)
print(f"{BLUE}" + testf + " - lorem returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")
testf = "test_crash_before_input.py"
print(f"{RED}" + testf + f" - lorem{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=lorem)
print(f"{BLUE}" + testf + " - lorem returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")

print(f"{RED}test_noread_input.py - lorem{RESET}")
testf = "test_noread_input.py"
print(f"{RED}" + testf + f" - lorem{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=lorem)
print(f"{BLUE}" + testf + " - lorem returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)



################# CHUNKED TEST #############################
testf = "test_post.py"
print(f"{RED}" + testf + f" - chunked{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=chunked)
print(f"{BLUE}" + testf + " - chunked returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")
testf = "test_infinite_loop_post.py"
print(f"{RED}" + testf + f" - chunked{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=chunked)
print(f"{BLUE}" + testf + " - chunked returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")
testf = "test_cookies_post.py"
print(f"{RED}" + testf + f" - chunked{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=chunked)
print(f"{BLUE}" + testf + " - chunked returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")
testf = "test_crash_after_input.py"
print(f"{RED}" + testf + f" - chunked{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=chunked)
print(f"{BLUE}" + testf + " - chunked returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)

input("press enter to continue")
testf = "test_crash_before_input.py"
print(f"{RED}" + testf + f" - chunked{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=chunked)
print(f"{BLUE}" + testf + " - chunked returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)


input("press enter to continue")
print(f"{RED}test_noread_input.py - chunked{RESET}")
testf = "test_noread_input.py"
print(f"{RED}" + testf + f" - chunked{RESET}")
response = requests.post(url + "cgi-bin/"+testf, data=chunked)
print(f"{BLUE}" + testf + " - chunked returned in " + str(response.elapsed.total_seconds()) + f"s{RESET}")
get_full_http_response_from_object(response)
