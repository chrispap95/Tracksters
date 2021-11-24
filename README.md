# Tracksters

Repository for HGCal projects in CMSSW

To install
```bash
cmsrel CMSSW_12_0_2
git clone git@github.com:chrispap95/Tracksters.git
scram b -j 4
```

To plot the graphs for CA and CLUE3D:
```bash
cd TrackstersTest/test
cmsrun getTrackstersInfoCA.py
cmsrun getTrackstersInfoCLUE3D.py
```

For different samples change lines 15 in getTrackstersInfoCA.py and getTrackstersInfoCLUE3D.py.

To run the centrality calculation:
```bash
cd TrackstersTest/test
cmsrun runCentrality.py
```
