#!/bin/sh

STR="haha\n haha"

CMP1=$(echo $STR |       cat - test.txt)
CMP2=$(echo $STR | ./bin/cat - test.txt)

if [ "$CMP1" = "$CMP2" ]; then
    echo "Test Passed"
else
    echo "Test Failed"
fi

CMP1=$(echo $STR |       wc -lcm test.txt - Makefile)
CMP2=$(echo $STR | ./bin/wc -lcm test.txt - Makefile)

if [ "$CMP1" = "$CMP2" ]; then
    echo "Test Passed"
else
    echo "Test Failed"
fi
