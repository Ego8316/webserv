#!/usr/bin/env python3

import os
import sys

# Function for minimal cookie value extraction
def get_cookie_value(cookie_string, key):
    if not cookie_string:
        return None
    parts = cookie_string.split(';')
    for part in parts:
        if '=' in part:
            k, v = part.strip().split('=', 1)
            if k.strip() == key:
                # Returns the value, handles cases where the value itself might contain spaces
                return v.strip()
    return None

def application():
    # 1. Identify where the raw cookie data is located (HTTP_COOKIE environment variable in CGI)
    cookie_header = os.environ.get('HTTP_COOKIE')
    cookie_name = "visit_count"
    
    current_count = 0
    
    if cookie_header:
        # Minimal parsing to find the specific cookie
        count_str = get_cookie_value(cookie_header, cookie_name)
        try:
            # Convert the found value to an integer
            current_count = int(count_str)
        except (ValueError, TypeError):
            # If the cookie is malformed or missing, reset the count
            current_count = 0

    # 2. Determine the new cookie value for the next request
    new_count = current_count + 1
    
    # Manually construct the Set-Cookie header string.
    # This is the raw header the server sends back.
    # Max-Age=3600 means the cookie will expire in 1 hour (3600 seconds).
    set_cookie_header = f"Set-Cookie: {cookie_name}={new_count}; Max-Age=3600; Path=/"

    # --- 3. Print Headers ---
    # The first line is the Content-type
    print("Content-type: text/html")
    # The second line is the Set-Cookie header we manually constructed
    print(set_cookie_header)
    # The critical blank line that separates HTTP headers from the body.
    print()

    # --- 4. Print HTML Body ---
    
    title = "Minimal CGI Cookie Test"
    
    if current_count == 0:
        message = "Welcome! This is your first visit (or the cookie expired/was blocked)."
        detail = f"A new cookie named '{cookie_name}' with value '1' has been manually set via the Set-Cookie header."
    else:
        message = f"Hello again! You have visited this page {new_count} times."
        detail = f"The cookie '{cookie_name}' (old value: {current_count}) has been updated to '{new_count}'."

    html_content = f"""
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

                <p><strong>2. Header Sent FROM Server (The manual <code>Set-Cookie</code> instruction):</strong></p>
                <pre>Content-type: text/html\n{set_cookie_header}</pre>
                
                <p>Refresh the page to see the count increase and the headers change.</p>
            </div>
        </div>
    </body>
    </html>
    """
    
    print(html_content)

if __name__ == "__main__":
    # Ensure stdout is flushed for immediate CGI response
    sys.stdout.reconfigure(encoding='utf-8')
    application()