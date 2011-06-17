#!/bin/bash

rm -rf files
mkdir files
cp ../../data files -r
cp ../../maps files -r
cp ../../manual files -r
cp ../../docs files -r
cp ../../*.otf files
cp ../lib/*.dll files
cp ../redist-msvc/*.dll files
cp ../redist-msvc/*.manifest files

rm -rf `find files -name ".svn"`
