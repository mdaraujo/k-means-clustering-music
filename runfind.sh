#!/bin/bash
for i in {1..7}
do
    ../bin-example/wavfind samples/cut_sample0$i.wav $1
    echo ""
done
