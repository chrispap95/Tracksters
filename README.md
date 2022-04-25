# Tracksters

Contains all the centrality related code.

To install
```bash
cmsrel CMSSW_X_Y_Z
cd CMSSW_X_Y_Z/src
git clone git@github.com:chrispap95/Tracksters.git
scram b -j 4
```
**Use CMSSW_12_4_0_pre2 as of April 2022**

The main algorithms reside in `TrackstersTest/src/TracksterUtilities.cc`.

There are three useful plugins as of now:
1) `TrackstersTest/plugins/Centrality.cc`: calculates centrality for a few different algorithms + weight scenarios and fills one histogram per layer.
2) `TrackstersTest/plugins/GraphDump.cc`: creates python files that contain that Trackster graph in a format that can be plotted by matplotlib/networkx. All the different centrality values for each node as well as the weight values for each edge are included in the output and can be plotted.
3) `TrackstersTest/plugins/TrackstersPrintout.cc`: simple code to printout any quantity you want for debugging purposes.

Below are a few typical workflows using the code in this repository.

## Generate debugging printouts
```bash
cd TrackstersTest/test
cmsRun trackstersPrintout_cfg.py
```
To print other values you can edit the code in `plugins/TrackstersPrintout.cc` and then
```bash
cd ${CMSSW_BASE}/Tracksters/TrackstersTest
scram b
cd test
cmsRun trackstersPrintout_cfg.py
```

## Plot trackster graphs
Run the following script on the sample to generate the python file with the graph information:
```bash
cd TrackstersTest/test
cmsRun dumpGraphs_cfg.py
```
This script is going to create (or update) the file named `plottingScripts/graphsCLUE3D.py`.
The script `plottingScripts/plotGraphs_single.py` can then be used to plot the graphs.
There is plenty of documentation for the use of that script on the header comments but a plain `python3 plotGraphs_single.py` should work right out of the box.

## Centrality
To generate the centrality distributions:
```bash
cd TrackstersTest/test
cmsRun runCentrality.py
```
This script is very customizable and accepts the following options:
```
  campaign           : camp6
                     - Value of campaign.
  cmssw              : CMSSW_12_0_2
                     - Value of CMSSW.
  delta              : 1p5
                     - Value of delta.
  directed           : 0
                     - Create directed or undirected graph.
  geometry           : D76
                     - Value of geometry.
  input              : 500
                     - Number of events.
  maxEvents          : -1
                     - Number of events to process (-1 for all)
  outputFile         : output.root
                     - Name of output file (if needed)
  tag                :
                     - tag to add to output filename
  weighted           : 0
                     - Creates weighted graph if not 0. Methods 1 through 3.
```
