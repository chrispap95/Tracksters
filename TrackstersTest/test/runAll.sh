#!/bin/bash

for i in 1p5 2 2p5 3 3p5 4 4p5;
do
  cmsRun runCentrality.py delta=${i}
done