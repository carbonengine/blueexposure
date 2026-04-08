# Copyright © 2026 CCP ehf.

import unittest


def _print_suite(test_suite_or_iterable):
    if not hasattr(test_suite_or_iterable, '__iter__'):
        print(test_suite_or_iterable.id())
    else:
        for item in test_suite_or_iterable:
            _print_suite(item)


def discover_tests():
    _print_suite(unittest.defaultTestLoader.discover('.'))
