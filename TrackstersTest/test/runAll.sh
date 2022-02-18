#!/bin/bash

for i in 1p5 2 2p5 3 3p5 4 4p5;
do
  for j in True False;
  do
    for k in 0 1 2 3 4;
    do
      cmsRun runCentrality.py delta=${i} cmssw=CMSSW_12_0_2 campaign=clue3D directed=${j} weighted=${k} &
    done
    wait
  done
done
