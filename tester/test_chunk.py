import subprocess

def runTestRequest(request_str):
	cmd = f"echo '{request_str}' | nc localhost 8080"
	process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, text=True)
	output, error = process.communicate()
	print(f"Output:\n{output}")


correct_request = "POST /cgi-bin/test_post.py HTTP/1.0\r\nTransfer-Encoding: chunked\r\n\r\n0x3\r\nLor\r\n0x8\r\nem ipsum\r\n0x7\r\n dolor \r\n0x7\r\nsit ame\r\n0x4\r\nt, c\r\n0x3\r\nons\r\n0x2\r\nec\r\n0xd\r\ntetur adipisc\r\n0x1\r\ni\r\n0x2\r\nng\r\n0x6\r\n elit.\r\n0x7\r\n Vivamu\r\n0x10\r\ns sit amet solli\r\n0x6\r\ncitudi\r\n0xd\r\nn libero. Qui\r\n0x7\r\nsque pe\r\n0xe\r\nllentesque pel\r\n0x8\r\nlentesqu\r\n0x0\r\n\r\n"
wrong_chunk_size = "POST /cgi-bin/test_post.py HTTP/1.0\r\nTransfer-Encoding: chunked\r\n\r\n0x3\r\nL\r\n0x8\r\nem ipsum\r\n0x7\r\n dolor \r\n0x7\r\nsit ame\r\n0x4\r\nt, c\r\n0x3\r\nons\r\n0x2\r\nec\r\n0xd\r\ntetur adipisc\r\n0x1\r\ni\r\n0x2\r\nng\r\n0x6\r\n elit.\r\n0x7\r\n Vivamu\r\n0x10\r\ns sit amet solli\r\n0x6\r\ncitudi\r\n0xd\r\nn libero. Qui\r\n0x7\r\nsque pe\r\n0xe\r\nllentesque pel\r\n0x8\r\nlentesqu\r\n0x0\r\n\r\n"
unfinished_request = "POST /cgi-bin/test_post.py HTTP/1.0\r\nTransfer-Encoding: chunked\r\n\r\n0x3\r\nLor\r\n0x8\r\nem ipsum\r\n0x7\r\n dolor \r\n0x7\r\nsit ame\r\n0x4\r\nt, c\r\n0x3\r\nons\r\n0x2\r\nec\r\n0xd\r\ntetur adipisc\r\n0x1\r\ni\r\n0x2\r\nng\r\n0x6\r\n elit.\r\n0x7\r\n Vivamu\r\n0x10\r\ns sit amet solli\r\n0x6\r\ncitudi\r\n0xd\r\nn libero. Qui\r\n0x7\r\nsque pe\r\n0xe\r\nllentesque pel\r\n"
unfinished_header_request = "POST /cgi-bin/test_post.py HTTP/1.0\r\nTransfer-Encoding: chunked\r\n"
print("Test: correct_request")
runTestRequest(correct_request)
input("press enter to continue")
print("Test: wrong_chunk_size")
runTestRequest(wrong_chunk_size)
input("press enter to continue")
print("Test: unfinished_request")
runTestRequest(unfinished_request)
input("press enter to continue")
print("Test: unfinished_header_request")
runTestRequest(unfinished_header_request)