#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/HGCalReco/interface/Trackster.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "Tracksters/TrackstersTest/interface/TracksterUtilities.h"

#include <Eigen/Core>
#include <Eigen/Dense>

class TrackstersPrintout : public edm::EDAnalyzer {
 public:
  explicit TrackstersPrintout(const edm::ParameterSet&);
  ~TrackstersPrintout() override;

 private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  edm::EDGetTokenT<std::vector<ticl::Trackster> > tracksters_;
  edm::EDGetTokenT<std::vector<reco::CaloCluster>> layerClusters_;
  bool isCLUE3D_;

};

std::ofstream fileout;

TrackstersPrintout::TrackstersPrintout(const edm::ParameterSet& iConfig) :
  isCLUE3D_(iConfig.getParameter<bool>("isCLUE3D")) {
  std::string trackstersTag = isCLUE3D_ ? "ticlTrackstersCLUE3DHigh" : "ticlTrackstersMerge";
  tracksters_ = consumes<std::vector<ticl::Trackster>>(edm::InputTag(trackstersTag));
  layerClusters_ = consumes<std::vector<reco::CaloCluster>>(edm::InputTag("hgcalLayerClusters"));
  std::string fileoutName = isCLUE3D_ ? "plottingScripts/graphsCLUE3D.py" : "plottingScripts/graphsCA.py";
  fileout.open(fileoutName);
}

TrackstersPrintout::~TrackstersPrintout() {}


void TrackstersPrintout::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  bool debug = false;
  edm::EventNumber_t evtNum = iEvent.eventAuxiliary().event();
  edm::Handle<std::vector<ticl::Trackster>> tracksterHandle;
  edm::Handle<std::vector<reco::CaloCluster>> layerClustersHandle;
  const std::vector<ticl::Trackster> *tracksters;
  const std::vector<reco::CaloCluster> *layerClusters;
  try {
    iEvent.getByToken(tracksters_, tracksterHandle);
    iEvent.getByToken(layerClusters_, layerClustersHandle);
    tracksters = &(*tracksterHandle);
    layerClusters = &(*layerClustersHandle);
  } catch (std::exception& ce) {
    std::cerr << "[TrackstersPrintout] caught std::exception " << ce.what() << std::endl;
    return;
  }

  // Keeps track of the number of tracksters in a single event
  size_t trkNum = 1;

  // Loop over the tracksters in the event
  for (std::vector<ticl::Trackster>::const_iterator it = tracksters->begin(); it != tracksters->end(); ++it) {
    const std::vector<unsigned int> &vertices = it->vertices();
    auto &edges = it->edges();
    auto seedIndex = it->seedIndex();

    // Initialize graph and pass seed index (to be implemented)
    fileout << "seedIndex" << evtNum << "_" << trkNum << " = " << seedIndex << "\n";
    fileout << "G" << evtNum << "_" << trkNum << " = nx.Graph()\n";

    // Populate nodes
    fileout << "G" << evtNum << "_" << trkNum << ".add_nodes_from([";
    for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
      fileout << *it_vert << ", ";
    }
    fileout << "])\n";

    // Populate edges
    fileout << "G" << evtNum << "_" << trkNum << ".add_edges_from([";
    for (auto edge : edges) {
      fileout << "(" << edge[0] << "," << edge[1] << ")" << ", ";
    }
    fileout << "])\n";

    // Get weights
    fileout << "weight1_" << evtNum << "_" << trkNum << " = {";
    for (auto edge : edges) {
      const auto cluster1 = layerClusters->at(edge[0]);
      const auto cluster2 = layerClusters->at(edge[1]);
      double weight = ticl::calcWeight(1, cluster1, cluster2, 1);
      fileout << "(" << edge[0] << "," << edge[1] << ") : " << weight << ", ";
    }
    fileout << "}\n";
    fileout << "weight2_" << evtNum << "_" << trkNum << " = {";
    for (auto edge : edges) {
      const auto cluster1 = layerClusters->at(edge[0]);
      const auto cluster2 = layerClusters->at(edge[1]);
      double weight = ticl::calcWeight(2, cluster1, cluster2, 1);
      fileout << "(" << edge[0] << "," << edge[1] << ") : " << weight << ", ";
    }
    fileout << "}\n";
    fileout << "weight3_" << evtNum << "_" << trkNum << " = {";
    for (auto edge : edges) {
      const auto cluster1 = layerClusters->at(edge[0]);
      const auto cluster2 = layerClusters->at(edge[1]);
      double weight = ticl::calcWeight(3, cluster1, cluster2, 1);
      fileout << "(" << edge[0] << "," << edge[1] << ") : " << weight << ", ";
    }
    fileout << "}\n";
    fileout << "weight4_" << evtNum << "_" << trkNum << " = {";
    for (auto edge : edges) {
      const auto cluster1 = layerClusters->at(edge[0]);
      const auto cluster2 = layerClusters->at(edge[1]);
      double weight = ticl::calcWeight(4, cluster1, cluster2, 1);
      fileout << "(" << edge[0] << "," << edge[1] << ") : " << weight << ", ";
    }
    fileout << "}\n";

    // Fill energies
    fileout << "energy_" << evtNum << "_" << trkNum << " = {";
    for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
      auto cluster = layerClusters->at(*it_vert);
      fileout << *it_vert << ": " << cluster.energy() << ", ";
    }
    fileout << "}\n";

    Eigen::MatrixXd adj[5];
    Eigen::VectorXd c_eig[5], c_katz[5], c_pr[5];

    for (size_t i = 0; i < 5; ++i) {
      // Fill adjacency matrix
      adj[i] = ticl::adjacency(i, false, vertices, edges, layerClusters);

      // Compute eigenvalues & eigenvectors
      c_eig[i] = ticl::centralityEigenvector(adj[i]);

      // Katz centrality calculation
      c_katz[i] = ticl::centralityKatz(adj[i]);

      // PageRank calculation
      c_pr[i] = ticl::centralityPageRank(adj[i]);

      // Send to output
      size_t j = 0;
      fileout << "cEig_wt" << i << "_" << evtNum << "_" << trkNum << " = {";
      for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
        fileout << *it_vert << ": " << c_eig[i][j] << ", ";
        ++j;
      }
      fileout << "}\n";
      j = 0;
      fileout << "cKatz_wt" << i << "_" << evtNum << "_" << trkNum << " = {";
      for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
        fileout << *it_vert << ": " << c_katz[i][j] << ", ";
        ++j;
      }
      fileout << "}\n";
      j = 0;
      fileout << "cPR_wt" << i << "_" << evtNum << "_" << trkNum << " = {";
      for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
        fileout << *it_vert << ": " << c_pr[i][j] << ", ";
        ++j;
      }
      fileout << "}\n";
    }

    trkNum++;
    fileout << "}\n";

    // Debugging stuff
    if (debug) {
      std::cout << "\nVertice layers: ";
      for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
        auto cluster = layerClusters->at(*it_vert);
        std::vector<std::pair<DetId, float>> clusterDetIds = cluster.hitsAndFractions();
        std::cout << "(" << HGCSiliconDetId(cluster.seed()).layer() << ",";
        std::cout << HGCSiliconDetId(clusterDetIds[0].first).layer() << "), ";
      }
    }
  }
}

void TrackstersPrintout::beginJob() {
  fileout << "import networkx as nx\n\n";
}

void TrackstersPrintout::endJob() {}

DEFINE_FWK_MODULE(TrackstersPrintout);
