#!/bin/bash

STR="haha\n haha"

print_seperator() {
    for i in `seq 1 $COLUMNS`; do
        printf -
    done
}

#testing cat
echo $STR | cat - test.txt
print_seperator
echo $STR | ./bin/cat - test.txt

print_seperator

#testing wc
echo $STR | wc -lcm test.txt - Makefile
print_seperator
echo $STR | ./bin/wc -lcm test.txt - Makefile
