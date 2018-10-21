#!/bin/bash
for i in {1..7}
do
    ../bin-example/wavcb samples/sample0$i.wav 2 0 128 20 5
    echo ""
done
