#!/bin/bash
for i in {1..7}
do
    ../bin-example/wavcb samples/sample0$i.wav 2 0 64 100 3
    echo ""
done
