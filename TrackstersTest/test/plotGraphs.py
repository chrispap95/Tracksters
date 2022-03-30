#
# This script plots the automatically generated graphs using networkx
#

import networkx as nx
import matplotlib.pyplot as plt
import graphsCA_1, graphsCLUE3D_1

subax1 = plt.subplot(121)
nx.draw(graphsCA_1.G5_1, with_labels=True, font_weight='bold')

subax2 = plt.subplot(122)
nx.draw(graphsCLUE3D_1.G5_1, with_labels=True, font_weight='bold')

plt.show()
