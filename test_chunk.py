import os

input_str = "POST /cgi-bin/test_post.py HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n0x3\r\nLor\r\n0x8\r\nem ipsum\r\n0x7\r\n dolor \r\n0x7\r\nsit ame\r\n0x4\r\nt, c\r\n0x3\r\nons\r\n0x2\r\nec\r\n0xd\r\ntetur adipisc\r\n0x1\r\ni\r\n0x2\r\nng\r\n0x6\r\n elit.\r\n0x7\r\n Vivamu\r\n0x10\r\ns sit amet solli\r\n0x6\r\ncitudi\r\n0xd\r\nn libero. Qui\r\n0x7\r\nsque pe\r\n0xe\r\nllentesque pel\r\n0x8\r\nlentesqu\r\n0x0"

print("(echo '" + input_str + "')| telnet localhost 8080")
os.system("(echo '" + input_str + "')| telnet localhost 8080")