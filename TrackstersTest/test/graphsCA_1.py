import networkx as nx

G4_1 = nx.Graph()
G4_1.add_nodes_from([7, 9, 12, 15, 20, 26, 32, 43, 48, 59, 68, 76, 84, 93, 100, 107, 114, 117, 130, ])
G4_1.add_edges_from([(7,12), (12,15), (12,20), (20,26), (26,32), (32,43), (43,68), (68,76), (68,84), (84,93), (84,100), (100,114), (100,117), (117,130), (84,107), (107,114), (114,130), (68,93), (93,100), (93,107), (93,114), (32,48), (48,59), (59,68), (59,76), (76,84), (76,93), (76,100), (59,84), (48,68), (48,76), (32,59), (26,48), (20,32), (20,43), (43,48), (43,59), (12,26), (26,43), (9,12), (15,26), (15,20), (9,20), (15,32), ])

G2_1 = nx.Graph()
G2_1.add_nodes_from([3, 6, 8, 10, 15, 17, 23, 30, 37, 42, 49, 54, 62, 71, 77, 80, 81, 86, 87, ])
G2_1.add_edges_from([(3,6), (6,8), (8,10), (8,15), (15,17), (17,37), (37,42), (37,49), (49,54), (54,62), (49,62), (37,54), (54,77), (77,81), (77,86), (77,87), (15,23), (23,30), (30,37), (30,49), (23,37), (23,42), (42,49), (49,71), (71,80), (42,54), (54,71), (42,62), (62,77), (62,81), (15,30), (30,42), (6,10), (10,15), (10,23), (6,15), (3,10), (10,17), (17,23), (17,30), (3,8), (8,17), ])

G3_1 = nx.Graph()
G3_1.add_nodes_from([3, 4, 7, 10, 13, 15, 20, 26, 31, 38, 42, 50, 59, 64, 73, 80, 88, 95, 110, ])
G3_1.add_edges_from([(3,4), (4,7), (7,10), (10,13), (10,15), (15,20), (20,26), (26,31), (26,38), (38,42), (38,50), (50,59), (59,64), (64,73), (73,80), (80,95), (95,110), (73,88), (88,95), (88,110), (73,95), (64,80), (80,88), (64,88), (59,73), (59,80), (50,64), (50,73), (38,59), (26,42), (42,50), (42,59), (42,64), (20,31), (31,38), (31,42), (31,50), (20,38), (15,26), (15,31), (10,20), (7,15), (4,10), (4,13), (13,15), (13,20), (7,13), (3,7), (3,10), ])

G1_1 = nx.Graph()
G1_1.add_nodes_from([5, 7, 12, 19, 20, 23, 28, 31, 43, 47, 56, 64, 72, 75, 82, 90, 97, ])
G1_1.add_edges_from([(5,12), (12,19), (19,23), (23,31), (31,47), (47,56), (56,72), (72,75), (56,75), (75,90), (47,72), (72,82), (72,90), (31,56), (56,64), (23,43), (43,47), (47,64), (64,75), (75,97), (64,82), (43,56), (43,64), (64,72), (12,20), (20,23), (20,28), (28,43), (28,47), (20,31), (12,23), (7,12), (19,20), (7,20), (19,28), (28,31), (7,19), (5,19), ])

G5_1 = nx.Graph()
G5_1.add_nodes_from([3, 7, 9, 17, 19, 21, 26, 35, 40, 48, 55, 61, 69, 70, 82, 88, 93, 99, 101, 107, 113, 114, 118, 123, 129, 130, ])
G5_1.add_edges_from([(3,9), (9,17), (17,21), (21,26), (26,35), (35,40), (35,48), (48,55), (55,61), (61,69), (61,82), (82,88), (82,93), (93,99), (93,107), (107,114), (114,118), (118,123), (114,123), (123,129), (123,130), (107,118), (55,69), (69,82), (69,88), (88,93), (88,99), (99,107), (99,113), (99,114), (88,101), (55,70), (48,61), (26,40), (40,48), (40,55), (21,35), (17,26), (9,21), (7,9), (9,19), (7,17), (17,19), ])

