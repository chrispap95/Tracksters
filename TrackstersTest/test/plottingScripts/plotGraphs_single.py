#
# This script plots the automatically generated graphs using networkx
# A single graph is plotted.
# There is also the ability to add a heatmap on the nodes (e.g. energy, centrality, etc)
# and labels on the edges (e.g. weights).
# Edit the input section. Select:
#     - evtNum: event number
#     - trkNum: number of trackster in the event (usually 1 or 2)
#     - weightMode: weight mode (ranges from 0 to 4):
#           0) 'no weight',
#           1) '$\max(E_1, E_2)$',
#           2) '$|E_2-E_1|$',
#           3) '$d^{-1}$',
#           4) '$|E_2-E_1|/d$'
#     - heatmapMode: what heatmap to apply to the nodes. Possible choices:
#           0) 'energy'
#           1) 'centrality_eigenvector'
#           2) 'centrality_katz'
#           3) 'centrality_pageRank'
#     - layout:
#           1) spring_layout: (default) Position nodes using Fruchterman-Reingold force-directed algorithm. Okay.
#           2) kamada_kawai_layout: Position nodes using Kamada-Kawai path-length cost-function. Probably the best.
#           3) spectral_layout: Position nodes using the eigenvectors of the graph Laplacian.
#     - node_size
#

import numpy as np
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from graphsCLUE3D import graphs

# Input section
evtNum = 5
trkNum = 1
weightMode = 0
heatmapMode = 1
layout = 2
node_size = 300
heatmapRangeMargin = 0.005 # Sets the lower and the upper margin for the range of the heatmap

# Define weight modes
weightNames = {
    0: ['weight0', 'no weight'],
    1: ['weight1', '$\max(E_1, E_2)$'],
    2: ['weight2', '$|E_2-E_1|$'],
    3: ['weight3', '$d^{-1}$'],
    4: ['weight4', '$|E_2-E_1|/d$']
}

# Define heatmap modes
heatmapNames = {
    0: ['energy', 'Energy [GeV]'],
    1: ['cEig', 'centrality$_{Eigenvector}$'],
    2: ['cKatz', 'centrality$_{Katz}$'],
    3: ['cPR', 'centrality$_{PageRank}$']
}
heatmapKey = heatmapNames[heatmapMode][0] + '_wt' + str(weightMode)

# Import stuff
graph = graphs[evtNum][trkNum]['graph']
edge_labels = None
if weightMode != 0:
    edge_labels = graphs[evtNum][trkNum][weightNames[weightMode][0]]
val_map = graphs[evtNum][trkNum][heatmapKey]

# Set canvas size
plt.rcParams['figure.figsize'] = [10.0, 8.0]
fig, ax = plt.subplots()
fig.tight_layout()

# Initialize stuff
# Layout
pos = None
if layout == 1:
    pos = nx.spring_layout(graph, k=0.2)
elif layout == 2:
    pos = nx.kamada_kawai_layout(graph)
elif layout == 3:
    pos = nx.spectral_layout(graph)
else:
    print('[Error]: layout %d not recognized. Reverting to Kamada-Kawai.'%(layout))
    pos = nx.kamada_kawai_layout(graph)
# Heatmap values
values = [val_map.get(node, 0.25) for node in graph.nodes()]
# Edge labels
if weightMode != 0:
    for edge in edge_labels:
        if (edge_labels[edge] < 0.2):
            edge_labels[edge] = round(edge_labels[edge],2)
        else:
            edge_labels[edge] = round(edge_labels[edge],1)

# Draw the graph
nx.draw(
    graph,
    pos=pos,
    cmap=plt.get_cmap('inferno'),
    node_color=values,
    vmin=0.0-heatmapRangeMargin,
    vmax=0.5+heatmapRangeMargin,
    with_labels=True,
    font_color='white',
    font_weight='bold',
    font_size=7,
    node_size=node_size
)
# Draw edge lables
if weightMode != 0:
    nx.draw_networkx_edge_labels(
        graph,
        pos=pos,
        edge_labels=edge_labels,
        font_color='black',
        font_weight='bold',
        font_size=8
    )
# Draw colorbar
mynodes=nx.draw_networkx_nodes(
    graph,
    pos=pos,
    cmap=plt.get_cmap('inferno'),
    vmin=0.0-heatmapRangeMargin,
    vmax=0.5+heatmapRangeMargin,
    node_color=values,
    node_size=node_size
)
cbar = fig.colorbar(mynodes)
cbar.set_label(heatmapNames[heatmapMode][1], rotation=270, fontsize=18, labelpad=40)

# Add some text
# build a rectangle in axes coords
left, width = .0, 1.
bottom, height = .0, 1.
center = left + width/2.
right = left + width
top = bottom + height
# and add to the canvas
ax.text(
    right,
    top-0.02,
    'Two $\gamma$, $E=25\,$GeV, $\eta=1.88$, $\delta=1.5\,$cm\nCMSSW_12_0_2, D76, CLUE3D',
    horizontalalignment='right',
    verticalalignment='bottom',
    transform=ax.transAxes,
    fontsize=12
)
ax.text(
    left,
    bottom,
    'Event %d, Trackster %d\nweights: %s'%(evtNum, trkNum, weightNames[weightMode][1]),
    horizontalalignment='left',
    verticalalignment='bottom',
    transform=ax.transAxes,
    fontsize=12
)

fig.savefig(
    'graphPlots/graphPlotCLUE3D_delta1p5_evt%d_trk%d_w%d_display%d.pdf'%(evtNum,trkNum,weightMode,heatmapMode),
    bbox_inches='tight'
)
plt.show()
