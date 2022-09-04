#!/usr/bin/env python3

"""Merge multiple JUnit XML results files into a single results file."""

#  MIT License
#  
#  Copyright (c) 2012 Corey Goldberg
#  
#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:
#  
#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.
#  
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.

import os
import sys
import xml.etree.ElementTree as ET


"""Merge multiple JUnit XML files into a single results file.

Output dumps to stdout.

example usage:
    $ python merge_junit_results.py directory > results.xml
"""


def main():
    args = sys.argv[1:]
    if not args:
        usage()
        sys.exit(2)
    if '-h' in args or '--help' in args:
        usage()
        sys.exit(2)
    xml_files = []
    for f in os.listdir(args[0]):
        if f.endswith("-Results.xml"):
            xml_files.append(os.path.join(args[0], f))

    merge_results(xml_files)


def merge_results(xml_files):
    failures = 0
    tests = 0
    errors = 0
    time = 0.0
    cases = []

    for file_name in xml_files:
        tree = ET.parse(file_name)
        test_suite = tree.getroot()
        failures += int(test_suite.attrib['failures'])
        tests += int(test_suite.attrib['tests'])
        errors += int(test_suite.attrib['errors'])
        time += float(test_suite.attrib['time'])
        cases.append(list(test_suite))

    new_root = ET.Element('testsuites')
    new_root.attrib['failures'] = '%s' % failures
    new_root.attrib['tests'] = '%s' % tests
    new_root.attrib['errors'] = '%s' % errors
    new_root.attrib['time'] = '%s' % time
    for case in cases:
        new_root.extend(case)
    new_tree = ET.ElementTree(new_root)
    ET.dump(new_tree)


def usage():
    this_file = os.path.basename(__file__)
    print ('Usage:  %s directory' % this_file)


if __name__ == '__main__':
    main()
