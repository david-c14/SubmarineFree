#!/bin/bash

grep Model::create * | sed 's/^[^\(]*(//' | awk 'BEGIN { FS="," } ; { gsub("^ *","",$2) ; print "\t\t"$2": {" ; print "\t\t\t\"name\":" $2 ","; print "\t\t\t\"description\":" $3 ","; a=""; for (i = 4; i <= NF; i++) { gsub("^ *","",$i); gsub("_.*$", "", $i); a = a"\""$i"\", " } ; gsub(", $", "", a) ; print "\t\t\t\"tags\": ["a"]" ; print "\t\t}," }'
