
from setuptools import setup, Extension
import os

nrghash = Extension(
    'nrghash',
    define_macros = [
        ('__STDC_WANT_LIB_EXT1__',1),
        ('USE_SECURE_MEMZERO',1),
    ],
    include_dirs = [
        'include',
    ],
    sources = [
        'nrghashmodule.cpp',
        'egihash.cpp',
    ],
    language='c++',
    extra_compile_args=['-std=c++11'])

setup(
    name = 'nrghash',
    version = '1.0.3',
    description = 'QuantisNet Hash function',
    ext_modules = [nrghash])
