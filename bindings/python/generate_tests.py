#! /bin/env python
"""
Creates tests for the Python bindings for Hamlib

Running this script directly will use the installed bindings.
For an in-tree run use "make check", or set PYTHONPATH to point to
the directories containing Hamlib.py and _Hamlib.so.
"""
import inspect
import os
import sys

import Hamlib

# This class gets copied to the generated file with replacements
class TestClass:
    """A pytest class for {object_name}"""

    @classmethod
    def setup_class(cls):
        """Common initialization before calling test methods"""
        cls.actual_callables, cls.actual_properties = cls.generate_data({object_name})

    def test_callables(self):
        """Check that nothing was added or removed"""
        expected_callables = {expected_callables}
        assert expected_callables == self.actual_callables

    def test_properties(self):
        """Check that nothing was added or removed"""
        expected_properties = {expected_properties}
        assert expected_properties == self.actual_properties

    @classmethod
    def generate_data(cls, the_object):
        """Extract callables and properties from the given object"""
        callables = []
        properties = []
        for method_or_property in dir(the_object):
            if not method_or_property.startswith("_"):
                if callable(getattr(the_object, method_or_property)):
                    callables.append(method_or_property)
                else:
                    properties.append(method_or_property)

        return callables, properties


def generate_file(object_name, callables, properties):
    """Create a complete file for pytest"""

    file_header = '''#! /bin/env pytest
"""
Tests of the Python bindings for Hamlib

DO NOT EDIT this autogenerated file, run "make generate-pytests" instead
"""
import Hamlib
'''
    filename = f"test_{object_name.replace(".", "_")}_class.py"
    class_source = inspect.getsource(TestClass)
    expected_callables = repr(callables)
    expected_properties = repr(properties)

    print(f"Generating {filename}")
    with open(filename, "w", encoding='utf-8') as output_file:
        print(file_header, file=output_file)
        print(class_source.format(
            object_name=object_name,
            expected_callables=expected_callables,
            expected_properties=expected_properties),
            file=output_file, end='')


if __name__ == "__main__":
    if len(sys.argv) == 2 and not sys.argv[1].startswith("-"):
        os.chdir(sys.argv[1])
    else:
        print("Usage: {sys.argv[0]} [OUTPUT_DIRECTORY]", file=sys.stderr)
        sys.exit(1)
    for the_object in (
        Hamlib,
        Hamlib.Amp,
        Hamlib.Rig,
        Hamlib.Rot,
    ):
        try:
            full_name = f"{the_object.__module__}.{the_object.__name__}"
        except AttributeError:
            full_name = the_object.__name__
        expected_callables, expected_properties = TestClass.generate_data(the_object)
        generate_file(full_name, expected_callables, expected_properties)
