#!/bin/sh

set -eu

mkdir Rack
unzip -o lin.zip -d Rack
unzip -o win.zip -d Rack
zip -q -9 -r combined.zip Rack
