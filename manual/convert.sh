#!/bin/bash

#apt-get install -y markdown

for p in *.md gates/*.md
do
	sed 's/\.md/\.html/g' ${p} | markdown > ${p%md}body
	cat prefix ${p%md}body suffix > ${p%md}html
done

git clone https://github.com/david-c14/Submarine.git

rm Submarine/Free/Manual/*
rm Submarine/Free/Manual/gates/*

cp * Submarine/Free/Manual
cp gates/* Submarine/Free/Manual/gates

rm Submarine/Free/Manual/*.md
rm Submarine/Free/Manual/*.body
rm Submarine/Free/Manual/gates/*.md
rm Submarine/Free/Manual/gates/*.body

cd Submarine
git add *
git commit -m 'Update SubmarineFree Manual'
git push
