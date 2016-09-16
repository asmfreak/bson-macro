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
    return int(log(abs(n), 256)) + 1

class EncodeTest(unittest.TestCase):
    def setUp(self):
        self._tempdir = tempfile.TemporaryDirectory()
        self.dir = self._tempdir.name
        self.target = os.path.join(self.dir, "test")
        self.data = [
            odict([("asas", 20)]),
            odict([("abaiksj", 200), ("kddsokn", 900), ("dsdsojok", 250)]),
            odict([("k4", 0xDEADBEEFFEED)]),
            odict([("k5", -0x0EADBEEFFEED)])
        ]
        self.compilers = ["gcc", "g++", "clang", "clang++"]

    def tearDown(self):
        self._tempdir.cleanup()

    def test_encode(self):
        for cc in self.compilers:
            for d in self.data:
                self.make_exe(self.target, lambda: self.generate_string(d), cc=cc)
                encoded = ''.join(["\\x{:x}".format(x) for x in bson.dumps(d)])
                encoded_c = check_output([self.target]).decode("utf-8")
                self.assertEqual(encoded, encoded_c, msg="test encode on {cc} data: {d}".format(cc=cc, d=d))

    def test_encode_two(self):
        for cc in self.compilers:
            for d in self.data:
                g = lambda: self.generate_string(d) + ", " + self.generate_string(d)
                self.make_exe(self.target, g, cc=cc)
                encoded = ''.join(["\\x{:x}".format(x) for x in bson.dumps(d)])
                encoded_c = check_output([self.target]).decode("utf-8")
                self.assertEqual(encoded+encoded, encoded_c, msg="test two on {cc} data: {d}".format(cc=cc, d=d))

    def test_decode(self):
        for cc in self.compilers:
            for d in self.data:
                self.make_exe(self.target, lambda: self.generate_string(d), cc=cc)
                encoded_c = map(lambda x: x.zfill(2) if x!='' else '' , check_output([self.target]).decode("utf-8").split("\\x"))
                encoded_c = bytes(bytearray.fromhex(" ".join(encoded_c)))
                self.assertEqual(d, bson.loads(encoded_c), msg="test decode on {cc} data: {d}".format(cc=cc, d=d))

    def make_exe(self, target, d, cc="gcc"):
        targetc = target + '.c'
        macro = d()
        self.make_file(targetc, macro)
        check_call([cc, targetc, "-I", os.path.dirname(os.path.abspath(__file__)), "-o", target])

    def generate_string(self, d):
        r = ""
        for k,v in reversed(d.items()):
            t = "INT32" if bytes_needed(v) <= 4 else "INT64"
            r += "{0}, \"{1}\", {2}, {3}, ".format(t, k, len(k), v)
        return "BSON_DOCUMENT({0}, {1})".format(len(d.keys()), r)

    def make_file(self, of, test_line):
        with open("test.ctemplate") as base, open(of, "w") as out: #, open("test.ccc", "w") as ff:
            for line in base:
                if line.startswith("//EDITHERE"):
                    out.write(test_line)
#                    ff.write(test_line)
                else:
                    out.write(line)
#                    ff.write(line)

if __name__ == "__main__":
    unittest.main()
