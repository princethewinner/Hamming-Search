#!/bin/bash
rm pyFastHammingSearch.cpython-37m-x86_64-linux-gnu.so
rm pyFastHammingSearch.cpp

python setup.py clean --all
python setup.py build_ext --inplace

python test_build.py