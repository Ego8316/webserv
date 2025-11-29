#!/usr/bin/env python3
import os
import sys
from datetime import datetime, timedelta

def headers():
    print("Content-Type: text/plain")
    expires = (datetime.utcnow() + timedelta(hours=6)).strftime("%a, %d %b %Y %H:%M:%S GMT")
    print(f"Set-Cookie: cgi_test_cookie=works; Path=/; Expires={expires}; SameSite=Lax")
    print("")

def body():
    cookies = {k: v for k, v in os.environ.items() if k.startswith('HTTP_COOKIE_')}
    content_length = int(os.environ.get("CONTENT_LENGTH", "0") or 0)
    payload = sys.stdin.read(content_length) if content_length > 0 else ""
    print("Hello from CGI!")
    print(f"Cookies seen: {cookies}")
    print(f"Payload: {payload!r}")

if __name__ == "__main__":
    headers()
    body()
