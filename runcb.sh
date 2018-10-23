#!/bin/bash
for i in {1..7}
do
    ../bin-example/wavcb samples/sample0$i.wav 16 0 256 100 3
    echo ""
done
