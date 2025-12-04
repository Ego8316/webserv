#! /usr/bin/env python3

import sys
import os

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
PARENT_DIR = os.path.dirname(BASE_DIR)

if PARENT_DIR not in sys.path:
    sys.path.insert(0, PARENT_DIR)

import unittest

if __name__ == "__main__":
    loader = unittest.TestLoader()
    suite = loader.discover("tester/tests")
    runner = unittest.TextTestRunner(verbosity=2)
    runner.run(suite)
