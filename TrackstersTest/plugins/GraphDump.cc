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

class GraphDump : public edm::EDAnalyzer {
 public:
  explicit GraphDump(const edm::ParameterSet&);
  ~GraphDump() override;

 private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  edm::EDGetTokenT<std::vector<ticl::Trackster> > tracksters_;
  edm::EDGetTokenT<std::vector<reco::CaloCluster>> layerClusters_;
  bool isCLUE3D_;

};

std::ofstream fileout;

GraphDump::GraphDump(const edm::ParameterSet& iConfig) :
  isCLUE3D_(iConfig.getParameter<bool>("isCLUE3D")) {
  std::string trackstersTag = isCLUE3D_ ? "ticlTrackstersCLUE3DHigh" : "ticlTrackstersMerge";
  tracksters_ = consumes<std::vector<ticl::Trackster>>(edm::InputTag(trackstersTag));
  layerClusters_ = consumes<std::vector<reco::CaloCluster>>(edm::InputTag("hgcalLayerClusters"));
  std::string fileoutName = isCLUE3D_ ? "plottingScripts/graphsCLUE3D.py" : "plottingScripts/graphsCA.py";
  fileout.open(fileoutName);
}

GraphDump::~GraphDump() {}


void GraphDump::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
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
    std::cerr << "[GraphDump] caught std::exception " << ce.what() << std::endl;
    return;
  }

  // Initialize graph dictionary for the event
  fileout << "graphs[" << evtNum << "] = {}\n";
  // Keeps track of the number of tracksters in a single event
  size_t trkNum = 1;

  // Loop over the tracksters in the event
  for (std::vector<ticl::Trackster>::const_iterator it = tracksters->begin(); it != tracksters->end(); ++it) {
    const std::vector<unsigned int> &vertices = it->vertices();
    auto &edges = it->edges();
    auto seedIndex = it->seedIndex();

    // Initialize graph and pass seed index
    fileout << "graphs[" << evtNum << "][" << trkNum << "] = {}\n";
    fileout << "graphs[" << evtNum << "][" << trkNum << "]['seedIndex'] = " << seedIndex << "\n";
    fileout << "graphs[" << evtNum << "][" << trkNum << "]['graph'] =  nx.Graph()\n";

    // Populate nodes
    fileout << "graphs[" << evtNum << "][" << trkNum << "]['graph'].add_nodes_from([";
    for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
      fileout << *it_vert << ", ";
    }
    fileout << "])\n";

    // Populate edges
    fileout << "graphs[" << evtNum << "][" << trkNum << "]['graph'].add_edges_from([";
    for (auto edge : edges) {
      fileout << "(" << edge[0] << "," << edge[1] << ")" << ", ";
    }
    fileout << "])\n";

    // Get weights
    for (size_t iW = 1; iW < 5; ++iW) {
      fileout << "graphs[" << evtNum << "][" << trkNum << "]['weight" << iW << "'] = {";
      for (auto edge : edges) {
        const auto cluster1 = layerClusters->at(edge[0]);
        const auto cluster2 = layerClusters->at(edge[1]);
        double weight = ticl::calcWeight(iW, cluster1, cluster2, 1);
        fileout << "(" << edge[0] << "," << edge[1] << ") : " << weight << ", ";
      }
      fileout << "}\n";
    }

    // Fill energies
    fileout << "graphs[" << evtNum << "][" << trkNum << "]['energy'] = {";
    for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
      auto cluster = layerClusters->at(*it_vert);
      fileout << *it_vert << ": " << cluster.energy() << ", ";
    }
    fileout << "}\n";

    Eigen::MatrixXd adj[5];
    Eigen::VectorXd c_eig[5], c_katz[5], c_pr[5];

    for (size_t iW = 0; iW < 5; ++iW) {
      // Fill adjacency matrix
      adj[iW] = ticl::adjacency(iW, false, vertices, edges, layerClusters);

      // Compute eigenvalues & eigenvectors
      c_eig[iW] = ticl::centralityEigenvector(adj[iW]);

      // Katz centrality calculation
      c_katz[iW] = ticl::centralityKatz(adj[iW]);

      // PageRank calculation
      c_pr[iW] = ticl::centralityPageRank(adj[iW]);

      // Send to output
      size_t j = 0;
      fileout << "graphs[" << evtNum << "][" << trkNum << "]['cEig_wt" << iW << "'] = {";
      for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
        fileout << *it_vert << ": " << c_eig[iW][j] << ", ";
        ++j;
      }
      fileout << "}\n";
      j = 0;
      fileout << "graphs[" << evtNum << "][" << trkNum << "]['cKatz_wt" << iW << "'] = {";
      for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
        fileout << *it_vert << ": " << c_katz[iW][j] << ", ";
        ++j;
      }
      fileout << "}\n";
      j = 0;
      fileout << "graphs[" << evtNum << "][" << trkNum << "]['cPR_wt" << iW << "'] = {";
      for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
        fileout << *it_vert << ": " << c_pr[iW][j] << ", ";
        ++j;
      }
      fileout << "}\n";
    }

    fileout << "\n";
    trkNum++;

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

void GraphDump::beginJob() {
  // Let's print a nice message at the top of the script
  fileout << "# This script has been generated automatically by the GraphDump plugin.\n";
  fileout << "# The main exportable object is be named 'graphs'. It is a list of lists with format\n";
  fileout << "# graphs[# of event][# of trackster in event]. Each entry is a dictionary that contains\n";
  fileout << "# entries for the graph, the energies, the weights, and the centralities.\n";
  fileout << "\nimport networkx as nx\n\n";
  fileout << "graphs = {}\n\n";
}

void GraphDump::endJob() {}

DEFINE_FWK_MODULE(GraphDump);
