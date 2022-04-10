#ifndef Tracksters_TrackstersTest_TracksterUtilities_H__
#define Tracksters_TrackstersTest_TracksterUtilities_H__

#include "DataFormats/HGCalReco/interface/Trackster.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"

#include <Eigen/Core>
#include <Eigen/Dense>

namespace ticl {
  double calcWeight(const int mode, const reco::CaloCluster& cluster1, const reco::CaloCluster& cluster2, const int n = 1);

  Eigen::MatrixXd adjacency(
      const int wtMode,
      const bool isDirected,
      const std::vector<unsigned int>& vertices,
      const std::vector<std::array<unsigned int, 2>>& edges,
      const std::vector<reco::CaloCluster>*& layerClusters);

  Eigen::VectorXd centralityEigenvector(const Eigen::MatrixXd& adj);

  Eigen::VectorXd centralityKatz(const Eigen::MatrixXd& adj);

  Eigen::VectorXd centralityPageRank(const Eigen::MatrixXd& adj, const float df = 0.85);
}  // namespace ticl

#endif
