#!/usr/bin/env python3

import os
import sys

def get_cookie_value(cookie_string, key):
    if not cookie_string:
        return None
    
    parts = cookie_string.split(';')
    for part in parts:
        if '=' in part:
            k, v = part.strip().split('=', 1)
            if k.strip() == key:
                return v.strip()
    return None

def application(input_str):
    cookie_header = os.environ.get('HTTP_COOKIE')
    cookie_name = "visit_count"
    
    current_count = 0
    
    if cookie_header:
        count_str = get_cookie_value(cookie_header, cookie_name)
        try:
            current_count = int(count_str)
        except (ValueError, TypeError):
            current_count = 0

    new_count = current_count + 1
    
    set_cookie_header = f"Set-Cookie: {cookie_name}={new_count}; Max-Age=3600; Path=/"
    content_type_header = "Content-type: text/html"

    title = "My first Cookies ! (chocolate chips ! not raisins...)"
    
    if current_count == 0:
        message = "Welcome! This is your first visit (or the cookie expired/was blocked)."
        detail = f"A new cookie named '{cookie_name}' with value '1' has been manually set via the Set-Cookie header."
    else:
        message = f"Hello again! You have visited this page {new_count} times."
        detail = f"The cookie '{cookie_name}' (old value: {current_count}) has been updated to '{new_count}'."
    content_length_display_placeholder = "####"
    
    html_content_template = f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>{title}</title>
        <style>
            body {{ font-family: 'Inter', sans-serif; margin: 40px; background-color: #e9ecef; }}
            .container {{ max-width: 600px; margin: auto; padding: 25px; border-radius: 8px; background-color: white; box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1); }}
            h1 {{ color: #007bff; border-bottom: 2px solid #007bff; padding-bottom: 10px; }}
            pre {{ background-color: #f8f9fa; padding: 15px; border-radius: 4px; overflow-x: auto; border: 1px solid #ced4da; font-size: 0.9em; }}
            .note {{ margin-top: 25px; padding: 15px; border-left: 5px solid #28a745; background-color: #d4edda; color: #155724; border-radius: 4px; }}
        </style>
    </head>
    <body>
        <div class="container">
            <h1>{title}</h1>
            <p><strong>{message}</strong></p>
            <p>{detail}</p>
            
            <div class="note">
                <h3>Debugging Information</h3>
                
                <p><strong>1. Header Sent TO Server (The raw <code>HTTP_COOKIE</code> read):</strong></p>
                <pre>{cookie_header if cookie_header else "No HTTP_COOKIE header received."}</pre>

                <p><strong>2. Headers Sent FROM Server (The manual headers):</strong></p>
                <pre>{content_type_header}\n{set_cookie_header}\nContent-Length: {content_length_display_placeholder}</pre>
                
                <p>Refresh the page to see the count increase and the headers change.</p>
            </div>
        </div>
        <div class="container">
            <h1>{title}</h1>
            <p><strong>Why did you sent me this ? what am I suppose to do with it ? a paper plane ? {input_str}</strong></p>
            <p>{detail}</p>
            
            <div class="note">
                <h3>Debugging Information</h3>
                
                <p><strong>1. Header Sent TO Server (The raw <code>HTTP_COOKIE</code> read):</strong></p>
                <pre>{cookie_header if cookie_header else "No HTTP_COOKIE header received."}</pre>

                <p><strong>2. Headers Sent FROM Server (The manual headers):</strong></p>
                <pre>{content_type_header}\n{set_cookie_header}\nContent-Length: {content_length_display_placeholder}</pre>
                
                <p>Refresh the page to see the count increase and the headers change.</p>
            </div>
        </div>
    </body>
    </html>
    """
    
    content_length = len(html_content_template.encode('utf-8'))
    content_length += (content_length > 9998) + (content_length > 99998) + (content_length > 999998) + (content_length > 9999998)
    content_length_header = f"Content-Length: {content_length}"
    
    html_content = html_content_template.replace(content_length_display_placeholder, str(content_length))
    print(content_type_header, end="\r\n")
    print(set_cookie_header, end="\r\n")
    print(content_length_header, end="\r\n")
    print("\r\n")
    print(html_content, end="\r\n")

if __name__ == "__main__":
    sys.stdout.reconfigure(encoding='utf-8')
    input_str = input()
    if (input_str.endswith("\r")):
        input_str = input_str[:-1]
    application(input_str)