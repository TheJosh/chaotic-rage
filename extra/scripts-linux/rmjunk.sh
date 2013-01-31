#!/bin/sh

find -name "*.blend1" | xargs svn rm --force
find -name "*.blend2" | xargs svn rm --force
find -name "*.mtl" | xargs svn rm --force
