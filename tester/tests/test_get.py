#! /usr/bin/env python3

import unittest
from tester.utils.client import get
from tester.config import PATHS

class   TestGET(unittest.TestCase):

    def test_get_existing_file(self):
        r = get(PATHS["existing_file"])
        assert r.status_code == 200

    def test_get_missing_file(self):
        r = get(PATHS["missing_file"])
        assert r.status_code == 404

    def test_get_forbidden_file(self):
        r = get(PATHS["forbidden_file"])
        print(PATHS["forbidden_file"])
        assert r.status_code == 403

if __name__ == "__main__":
    unittest.main()
