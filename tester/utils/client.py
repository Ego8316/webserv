#! /usr/bin/env python3

import requests
from tester.config import BASE_URL

def get(path, **kwargs):
    return requests.get(BASE_URL + path, **kwargs)

def post(path, **kwargs):
    return requests.post(BASE_URL + path, **kwargs)

def delete(path, **kwargs):
    return requests.delete(BASE_URL + path, **kwargs)
