#! /usr/bin/env python3

BASE_URL = "http://localhost:8081"

PATHS = {
    "existing_file": "/index.html",
    "missing_file": "/no_such_file",
    "forbidden_file": "/forbidden.txt",
    "static": "/static",
    "upload": "/upload",
    "redirect": "/redirect",
}

MAX_UPLOAD_SIZE = 2 * 1024 * 1024
TIMEOUT_SECONDS = 1
