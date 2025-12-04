#!/usr/bin/env python3

import os
import http.cookies
import sys # <-- Added to read from STDIN

# --- 1. Set Content-Type header
print("Content-Type: text/html\r\n", end="")

# --- 2. Set a Test Cookie (for the browser to send on the next request)
cookie_to_set = http.cookies.SimpleCookie()
cookie_to_set["cgi_test_key"] = "CGI_Cookie_Success"
cookie_to_set["cgi_test_key"]["expires"] = 30 * 24 * 3600 # 30 days
cookie_to_set["cgi_test_key"]["path"] = "/"

# Print Set-Cookie header
print(cookie_to_set.output(), end="")

# --- 3. Print the blank line to end the headers and start the body
print("\r\n", end="")

# --- 4. Start of minimal HTML Body
print("""
<!DOCTYPE html>
<html>
<head>
    <title>Minimal CGI Cookie & STDIN Test</title>
</head>
<body>
    <h1>Minimal CGI Cookie & STDIN Tester</h1>
""")

# --- STDIN Reading Logic (New)
# This checks for the CONTENT_LENGTH environment variable (set by the server)
# and reads that many bytes from the standard input pipe.
content_length_str = os.environ.get('CONTENT_LENGTH', '0')
content_length = 0

try:
    content_length = int(content_length_str)
except ValueError:
    pass # content_length remains 0

print("<h2>STDIN Input (Request Body):</h2>")
print(f"<p>CONTENT_LENGTH: <b>{content_length}</b> bytes</p>")

if content_length > 0:
    try:
        # Read the exact number of bytes from STDIN and decode as UTF-8
        stdin_data = sys.stdin.read(content_length).decode('utf-8')
        print(f"<p>Received Data:")
        print(f"<pre style='border: 1px solid #ccc; padding: 10px;'>{stdin_data}</pre></p>")
    except Exception as e:
        print(f"<p style='color:red;'>Error reading STDIN: {e}</p>")
else:
    print("<p>No data received via STDIN. (Typically occurs for GET requests or when CONTENT_LENGTH is missing/zero.)</p>")

print("<hr>")

# --- 5. Read the incoming cookie data from HTTP_COOKIE environment variable
cookie_string = os.environ.get('HTTP_COOKIE', None)

if cookie_string:
    print(f"<h2>Cookies Received:</h2>")
    print(f"<p>Raw HTTP_COOKIE: <pre>{cookie_string}</pre></p>")

    try:
        incoming_cookies = http.cookies.SimpleCookie(cookie_string)
        print(f"<h3>Parsed Cookies:</h3>")
        print("<ul>")
        for key, morsel in incoming_cookies.items():
            print(f"<li>Key: <b>{key}</b> | Value: <b>{morsel.value}</b></li>")
        print("</ul>")

        # --- 6. Check for the newly set test cookie (on second visit)
        if "cgi_test_key" in incoming_cookies:
            print(f"<p style='color:green;'>SUCCESS: Test cookie 'cgi_test_key' received.</p>")
        else:
            print(f"<p style='color:orange;'>NOTICE: Test cookie 'cgi_test_key' not found. Please refresh the page.</p>")

    except Exception as e:
        print(f"<p style='color:red;'>Error parsing cookies: {e}</p>")

else:
    print(f"<h2>No Cookies Received:</h2>")
    print("<p>A cookie has been set. Please refresh this page (visit it again) to see it appear.</p>")


# --- 7. Final Output
print("""
    <hr>
    <p>A 'Set-Cookie' header was sent to your browser for 'cgi_test_key'.</p>
</body>
</html>
""")
