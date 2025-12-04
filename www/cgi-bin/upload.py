#!/usr/bin/python3

import cgi, os

form = cgi.FieldStorage()

# Check if the field exists
if "filename" not in form:
    print("Content-Type: text/html", end = "\r\n\r\n")

    print("<h1>No file uploaded</h1>")

fileitem = form["filename"]

if fileitem.filename:
    upload_dir = os.getcwd() + '/tmp/'
    filepath = os.path.join(upload_dir, os.path.basename(fileitem.filename))

    with open(filepath, 'wb') as f:
        f.write(fileitem.file.read())

    message = f'File "{os.path.basename(fileitem.filename)}" uploaded to {upload_dir}'
else:
    message = 'Uploading failed'

print("Content-Type: text/html; charset=utf-8", end = "\r\n\r\n")
print(f"<h1>{message}</h1>")
