import lit.formats
import os

config.test_source_root = os.path.dirname(__file__)
config.name = "zip"
config.suffixes = [".cpp"]
config.test_format = lit.formats.ShTest()
