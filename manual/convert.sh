#!/bin/bash

#apt-get install -y markdown

git clone https://github.com/david-c14/Submarine.git

rm Submarine/Free/Manual/*.html
rm Submarine/Free/Manual/*.png
rm Submarine/Free/Manual/gates/*

for p in *.md
do
	sed 's/\.md/\.html/g' ${p} | markdown > ${p%md}body
	cat Submarine/Free/Manual/prefix ${p%md}body Submarine/Free/Manual/suffix > Submarine/Free/Manual/${p%md}html
done

cp *.png Submarine/Free/Manual
cp gates/* Submarine/Free/Manual/gates

rm *.body

cd Submarine
git add *
git commit -m 'Update SubmarineFree Manual'
git push
