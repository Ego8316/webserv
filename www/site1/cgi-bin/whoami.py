#!/usr/bin/env python3

import os
import sys
import uuid
import time

clientDB = "./DB/sessions.txt"
session_field = "session_id"

def get_cookie(cookie_string, key):
    if not cookie_string:
        return None
    for part in cookie_string.split(';'):
        if '=' in part:
            k, v = part.strip().split('=', 1)
            if k == key:
                return v
    return None

def load_clientDB():
    sessions = {}
    try:
        with open(clientDB, 'r') as f:
            for line in f:
                if ':' in line:
                    sid, count_str = line.strip().split(':', 1)
                    sessions[sid] = int(count_str)
    except FileNotFoundError:
        pass
    except ValueError:
        pass
    return sessions

def save_session_data(sessions):
    try:
        with open(clientDB, 'w') as f:
            for sid, count in sessions.items():
                f.write(f"{sid}:{count}\n")
    except IOError as e:
        print(f"Content-type: text/plain\r\n\r\nError saving session data: {e}", file=sys.stderr)
        sys.exit(1)

def application():
    cookie_header = os.environ.get('HTTP_COOKIE', '')
    client_sid = get_cookie(cookie_header, session_field)
    sessions = load_clientDB()
    if client_sid and client_sid in sessions:
        current_count = sessions[client_sid]
        new_sid = client_sid
    else:
        new_sid = str(uuid.uuid4())
        current_count = 0
    new_count = current_count + 1
    sessions[new_sid] = new_count
    save_session_data(sessions)
    set_cookie_header = f"Set-Cookie: {session_field}={new_sid}; Max-Age=3600; Path=/"
    content_type_header = "Content-type: text/html; charset=utf-8"
    title = "Simple CGI Session Test"
    
    if current_count == 0:
        message = f"Welcome! I don't recall ever seeing you before. Here's a Session ID just for you ! {new_sid}"
        detail = "A new cookie has been set. Refresh the page to see the count increase."
    else:
        message = f"Hi hi hi Mr Deltoid ! Funny surprise seeing you here ! (for the {new_count} times...)."
        detail = f"Your number of visit has been updated (old value: {current_count}). Session ID: {new_sid}. Please DO HESITATE to come back !"
        
    html_content = f"""
<!DOCTYPE html>
<html>
<head><title>{title}</title></head>
<body>
    <h1>{title}</h1>
    <p><strong>{message}</strong></p>
    <p>{detail}</p>
    <p><em>Data are stored in {clientDB} on the server. (MY database ! Dont go near it ! Grrrrr)</em></p>
</body>
</html>
"""
    body_bytes = html_content.encode('utf-8')
    content_length_header = f"Content-Length: {len(body_bytes)}"
    print(content_type_header, end="\r\n")
    print(set_cookie_header, end="\r\n")
    print(content_length_header, end="\r\n")
    print("\r\n", end="")
    print(body_bytes, end="\r\n")


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding='utf-8')
    application()
