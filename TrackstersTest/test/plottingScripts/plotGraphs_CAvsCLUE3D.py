#
# This script plots the automatically generated graphs using networkx
# Possible layouts:
#     - spring_layout: (default) Position nodes using Fruchterman-Reingold force-directed algorithm. Okay.
#     - kamada_kawai_layout: Position nodes using Kamada-Kawai path-length cost-function. Probably the best.
#     - spectral_layout: Position nodes using the eigenvectors of the graph Laplacian.

import networkx as nx
import matplotlib.pyplot as plt
import graphsCA, graphsCLUE3D

subax1 = plt.subplot(121)
nx.draw(graphsCA.G5_1, pos=nx.spring_layout(graphsCA.G5_1, k=0.2), with_labels=True, font_weight='bold')

subax2 = plt.subplot(122)
nx.draw(graphsCLUE3D.G5_1, pos=nx.spring_layout(graphsCLUE3D.G5_1, k=0.2), with_labels=True, font_weight='bold')

plt.show()
