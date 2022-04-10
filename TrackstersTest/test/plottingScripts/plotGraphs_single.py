#
# This script plots the automatically generated graphs using networkx
# A single graph is plotted.
# Possible layouts:
#     - spring_layout: (default) Position nodes using Fruchterman-Reingold force-directed algorithm. Okay.
#     - kamada_kawai_layout: Position nodes using Kamada-Kawai path-length cost-function. Probably the best.
#     - spectral_layout: Position nodes using the eigenvectors of the graph Laplacian.
# There is also the ability to add a heatmap on the nodes (e.g. energy, centrality, etc)
# and labels on the edges (e.g. weights).

import numpy as np
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import graphsCLUE3D as grp

# some info
evtNum = 5
trkNum = 1
weightMode = 4
weightNames = {
    0: 'no weight',
    1: '$\max(E_1, E_2)$',
    2: '$|E_2-E_1|$',
    3: '$d^{-1}$',
    4: '$|E_2-E_1|/d$'
}

# Set canvas size
plt.rcParams['figure.figsize'] = [10.0, 8.0]
fig, ax = plt.subplots()
fig.tight_layout()

# Import and initialize stuff
graph = grp.G5_1
edge_labels = grp.weight4_5_1
val_map = grp.energy_5_1
pos = nx.kamada_kawai_layout(graph)
values = [val_map.get(node, 0.25) for node in graph.nodes()]
for edge in edge_labels:
    if (edge_labels[edge] < 0.2):
        edge_labels[edge] = round(edge_labels[edge],2)
    else:
        edge_labels[edge] = round(edge_labels[edge],1)
node_size = 300

# Draw the graph
nx.draw(
    graph,
    pos=pos,
    cmap=plt.get_cmap('inferno'),
    node_color=values,
    with_labels=True,
    font_color='white',
    font_weight='bold',
    font_size=7,
    node_size=node_size
)
# Draw edge lables
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
    node_color=values,
    node_size=node_size
)
cbar = fig.colorbar(mynodes)
cbar.set_label('Energy [GeV]', rotation=270, fontsize=18, labelpad=40)

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
    'Event %d, Trackster %d\nweights: %s'%(evtNum, trkNum, weightNames[weightMode]),
    horizontalalignment='left',
    verticalalignment='bottom',
    transform=ax.transAxes,
    fontsize=12
)

fig.savefig('graphPlots/graphPlotCLUE3D_delta1p5_evt%d_trk%d_w%d.pdf'%(evtNum,trkNum,weightMode),bbox_inches='tight')
plt.show()
