# Tracksters

Repository for HGCal projects in CMSSW

To install
```bash
cmsrel CMSSW_X_Y_Z
cd CMSSW_X_Y_Z/src
git clone git@github.com:chrispap95/Tracksters.git
scram b -j 4
```

There are three useful plugins as of now:
1) `TrackstersTest/plugins/Centrality.cc`: calculates centrality for a few different algorithms + weight scenarios.
2) `TrackstersTest/plugins/TrackstersTestCA.cc`: prints out details about the tracksters (for CA algorithm).
3) `TrackstersTest/plugins/TrackstersTestCLUE3D.cc`: prints out details about the tracksters (for CLUE3D algorithm).

Below are a few typical workflows using the code in this repository.

## Plot trackster graphs
First, run the following scripts on the samples to print out the trackster details:
```bash
cd TrackstersTest/test
cmsRun getTrackstersInfoCA.py
cmsRun getTrackstersInfoCLUE3D.py
```
These scripts are going to create the files `graphsCA.py` and `graphsCLUE3D.py`.
The script `plotGraphs.py` can then be used to plot the graphs.

## Centrality
To run the centrality calculation:
```bash
cd TrackstersTest/test
cmsRun runCentrality.py
```
This script is very customizable and accepts the following options:
```
Singletons:
  campaign           : camp6
                     - Value of campaign.
  cmssw              : CMSSW_12_0_2
                     - Value of CMSSW.
  delta              : 1p5
                     - Value of delta.
  directed           : 0
                     - Create directed or undirected graph.
  filePrepend        :
                     - String to prepend location of all files
  geometry           : D76
                     - Value of geometry.
  input              : 500
                     - Number of events.
  maxEvents          : -1
                     - Number of events to process (-1 for all)
  outputFile         : output.root
                     - Name of output file (if needed)
  secondaryOutputFile:
                     - Name of second output file (if needed)
  section            : 0
                     - This section (from 1..totalSections inclusive)
  storePrepend       :
                     - Prepend location of files starting with '/store'
  tag                :
                     - tag to add to output filename
  totalSections      : 0
                     - Total number of sections
  weighted           : 0
                     - Creates weighted graph if not 0. Methods 1 through 3.
```
