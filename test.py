import os
import tempfile
import unittest
import bson
from collections import OrderedDict as odict
from subprocess import check_call, check_output
from math import log

def bytes_needed(n):
    if n == 0:
        return 1
    return int(log(n, 256)) + 1

class EncodeTest(unittest.TestCase):
    def setUp(self):
        self._tempdir = tempfile.TemporaryDirectory()
        self.dir = self._tempdir.name
        self.target = os.path.join(self.dir, "test")
        self.data = [
            odict([("asas", 20)]),
            odict([("abaiksj", 200), ("kddsokn", 900), ("dsdsojok", 250)]),
            odict([("k4", 0xDEADBEEFFEED)])
        ]

    def tearDown(self):
        self._tempdir.cleanup()

    def test_encode(self):
        for d in self.data:
            self.make_exe(self.target, d)
            encoded = ''.join(["\\x{:x}".format(x) for x in bson.dumps(d)])
            encoded_c = check_output([self.target]).decode("utf-8")
            self.assertEqual(encoded, encoded_c)

    def test_decode(self):
        for d in self.data:
            self.make_exe(self.target, d)
            encoded_c = map(lambda x: x.zfill(2) if x!='' else '' , check_output([self.target]).decode("utf-8").split("\\x"))
            encoded_c = bytes(bytearray.fromhex(" ".join(encoded_c)))
            self.assertEqual(d, bson.loads(encoded_c))

    def make_exe(self, target, d):
        targetc = target + '.c'
        macro = self.generate_string(d)
        self.make_file(targetc, macro)
        check_call(["gcc", targetc, "-I", os.path.dirname(os.path.abspath(__file__)), "-o", target])

    def generate_string(self, d):
        r = ""
        for k,v in reversed(d.items()):
            t = "INT32" if bytes_needed(v) <= 4 else "INT64"
            r += "{0}, \"{1}\", {2}, {3}, ".format(t, k, len(k), v)
        return "EVAL(DOCUMENT({0}, {1}))".format(len(d.keys()), r)

    def make_file(self, of, test_line):
        with open("test.c") as base, open(of, "w") as out:
            for line in base:
                if line.startswith("//EDITHERE"):
                    out.write(test_line)
                else:
                    out.write(line)

if __name__ == "__main__":
    unittest.main()
