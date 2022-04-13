#include "Tracksters/TrackstersTest/interface/TracksterUtilities.h"

namespace ticl {
  double calcWeight(const int mode, const reco::CaloCluster& cluster1, const reco::CaloCluster& cluster2, const int n) {
    double weight;
    const double E1 = cluster1.energy();
    const double E2 = cluster2.energy();
    const double dx = cluster2.x()-cluster1.x();
    const double dy = cluster2.y()-cluster1.y();
    const double dz = cluster2.z()-cluster1.z();
    const double dist = sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));
    /*
    ** Default is not weighted
    ** Method 1: max(E1, E2)
    ** Method 2: |E1 - E2|
    ** Method 3: (d)^(-n)
    ** Method 4: (method 1 or 2)*(method 3)
    */
    switch (mode) {
      default:
        weight = 1;
        break;
      case 1:
        weight = std::max(E1, E2);
        break;
      case 2:
        weight = abs(E1-E2);
        break;
      case 3:
        weight = pow(dist,-n);
        break;
      case 4:
        weight = abs(E1-E2)*pow(dist,-n);
        break;
    }
    return weight;
  }

  Eigen::MatrixXd adjacency(
      const int wtMode,
      const bool isDirected,
      const std::vector<unsigned int>& vertices,
      const std::vector<std::array<unsigned int, 2>>& edges,
      const std::vector<reco::CaloCluster>*& layerClusters) {
    // Define commonly used objects
    const size_t nV = vertices.size();
    Eigen::MatrixXd adj = Eigen::MatrixXd::Zero(nV,nV);
    // Fill adjacency matrix
    for (auto edge : edges) {
      const unsigned idx0 = std::find(vertices.begin(),vertices.end(),edge[0]) - vertices.begin();
      const unsigned idx1 = std::find(vertices.begin(),vertices.end(),edge[1]) - vertices.begin();
      if (idx0 == nV || idx1 == nV) {
        std::cout << "Warning: vertice is out of bounds.\n";
        continue;
      }
      const auto cluster1 = layerClusters->at(edge[0]);
      const auto cluster2 = layerClusters->at(edge[1]);
      const double weight = ticl::calcWeight(wtMode, cluster1, cluster2, 1);
      adj(idx0,idx1) = weight;
      if (!isDirected) adj(idx1,idx0) = weight;
    }
    return adj;
  }

  Eigen::VectorXd centralityEigenvector(const Eigen::MatrixXd& adj) {
    const size_t nV = adj.rows();
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(adj);
    if (eigensolver.info() != Eigen::Success) abort();
    if (eigensolver.eigenvalues()[nV-1] == 0) std::cout << "[centralityEigenvector] Warning: trivial solution\n";
    auto c_eig = (Eigen::VectorXd)eigensolver.eigenvectors().col(nV-1);
    if (c_eig(0) < 0) c_eig *= -1;
    c_eig.normalize();
    return c_eig;
  }

  Eigen::VectorXd centralityKatz(const Eigen::MatrixXd& adj) {
    const size_t nV = adj.rows();
    Eigen::VectorXd v_ones = Eigen::VectorXd::Ones(nV);
    Eigen::MatrixXd id = Eigen::MatrixXd::Identity(nV,nV);
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(adj);
    if (eigensolver.info() != Eigen::Success) abort();
    if (eigensolver.eigenvalues()[nV-1] == 0) std::cout << "[centralityKatz] Warning: trivial solution\n";
    const float alpha = 0.9/eigensolver.eigenvalues()[nV-1]; // has to be smaller than 1/(largest eigenvalue)
    auto c_katz = (Eigen::VectorXd)(((id-alpha*adj.transpose()).inverse()-id)*v_ones);
    if (c_katz[0] < 0) c_katz *= -1;
    c_katz.normalize();
    return c_katz;
  }

  Eigen::VectorXd centralityPageRank(const Eigen::MatrixXd& adj, const float df) {
    const size_t nV = adj.rows();
    Eigen::MatrixXd m_ones = Eigen::MatrixXd::Ones(nV,nV);
    auto m_pr = df*adj+(1-df)*m_ones/nV;
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver_pr(m_pr);
    if (eigensolver_pr.info() != Eigen::Success) abort();
    if (eigensolver_pr.eigenvalues()[nV-1] == 0) std::cout << "[centralityPageRank] Warning: trivial solution\n";
    auto c_pr = (Eigen::VectorXd)eigensolver_pr.eigenvectors().col(nV-1);
    if (c_pr[0] < 0) c_pr *= -1;
    c_pr.normalize();
    return c_pr;
  }
}  // namespace ticl
