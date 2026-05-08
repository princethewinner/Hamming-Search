#!/usr/bin/env python3
#encoding: UTF-8

from distutils.core import setup, Extension
from Cython.Build import cythonize

import os


os.environ['CFLAGS'] = f' -g -std=c++0x -O3 -march=native'

setup(ext_modules = cythonize(Extension(f'pyFastHammingSearch',
                                        sources=['CompressTree.cpp', 'FastHammingSearch.cpp', 'HammingTree.cpp', 'Timer.cpp', 'utils.cpp', 'memusage.cpp', 'pyFastHammingSearch.pyx'],
                                        language='c++',
                                        libraries=['z', 'm', 'boost_iostreams'])),
        version=1, author='Aashi Jindal & Prashant Gupta', name=f'pyFastHammingSearch',  language_level='3')
