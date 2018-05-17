#!/bin/bash

rm -R test-tree

mkdir -p test-tree/a/hawkwind
echo "silver-machine" >test-tree/a/hawkwind/silver-machine.txt
touch -m -t 200001011200 test-tree/a/hawkwind/silver-machine.txt
echo "urban-guerilla" >test-tree/a/hawkwind/urban-guerilla.txt
touch -m -t 200001011201 test-tree/a/hawkwind/urban-guerilla.txt
mkdir -p test-tree/a/robert-johnson
echo "cross-road-blues" >test-tree/a/robert-johnson/cross-road-blues.txt
touch -m -t 200001011201 test-tree/a/robert-johnson/cross-road-blues.txt
./durruti -t test-tree/tree-a-past.durruti test-tree/a

echo "the-watcher" >test-tree/a/hawkwind/the-watcher.txt
#dd if=/dev/urandom of=test-tree/a/hawkwind/the-watcher.txt bs=10M count=50
touch -m -t 200001011205 test-tree/a/hawkwind/the-watcher.txt
echo "love-in-vain" >test-tree/a/robert-johnson/love-in-vain.txt
touch -m -t 200001011201 test-tree/a/robert-johnson/love-in-vain.txt
./durruti -t test-tree/tree-a-now.durruti test-tree/a


mkdir -p test-tree/b/hawkwind
echo "silver-machine" >test-tree/b/hawkwind/silver-machine.txt
touch -m -t 200001011200 test-tree/b/hawkwind/silver-machine.txt
echo "urban-guerilla" >test-tree/b/hawkwind/urban-guerilla.txt
touch -m -t 200001011201 test-tree/b/hawkwind/urban-guerilla.txt
mkdir -p test-tree/b/robert-johnson
echo "cross-road-blues" >test-tree/b/robert-johnson/cross-road-blues.txt
touch -m -t 200001011201 test-tree/b/robert-johnson/cross-road-blues.txt
./durruti -t test-tree/tree-b-past.durruti test-tree/b

echo "kings-of-speed" >test-tree/b/hawkwind/kings-of-speed.txt
touch -m -t 200001011201 test-tree/b/hawkwind/kings-of-speed.txt
echo "hurry-on-sundown" >test-tree/b/hawkwind/hurry-on-sundown.txt
touch -m -t 200001011205 test-tree/b/hawkwind/hurry-on-sundown.txt
echo "love-in-vain" >test-tree/b/robert-johnson/love-in-vain.txt
touch -m -t 200001011201 test-tree/b/robert-johnson/love-in-vain.txt
echo "urban-guerilla" >test-tree/b/robert-johnson/urban-guerilla.txt
touch -m -t 200101011201 test-tree/b/hawkwind/urban-guerilla.txt
./durruti -t test-tree/tree-b-now.durruti test-tree/b

# get changes on each side
./durruti -o test-tree/tree-a-past.durruti -n test-tree/tree-a-now.durruti -c test-tree/changes-tree-a.durruti
./durruti -o test-tree/tree-b-past.durruti -n test-tree/tree-b-now.durruti -c test-tree/changes-tree-b.durruti

# compare both sides
echo ./durruti -a none -o test-tree/changes-tree-a.durruti -n test-tree/changes-tree-b.durruti
./durruti -a none -o test-tree/changes-tree-a.durruti -n test-tree/changes-tree-b.durruti

#read

#rm -R test-tree


# TODO: Test-cases
# - same directory, one file the-wachter.txt changed in A, one file urban-guerilla.txt changed in B -> bug was that those have been treated as conflict
