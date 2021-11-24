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

class TrackstersTestCLUE3D : public edm::EDAnalyzer {
 public:
  explicit TrackstersTestCLUE3D(const edm::ParameterSet&);
  ~TrackstersTestCLUE3D() override;

 private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  edm::EDGetTokenT<std::vector<ticl::Trackster> > tracksters_;
  edm::EDGetTokenT<std::vector<reco::CaloCluster>> layerClusters_;

};

TrackstersTestCLUE3D::TrackstersTestCLUE3D(const edm::ParameterSet& iConfig) {
  tracksters_ = consumes<std::vector<ticl::Trackster>>(edm::InputTag("ticlTrackstersCLUE3DHigh"));
  layerClusters_ = consumes<std::vector<reco::CaloCluster>>(edm::InputTag("hgcalLayerClusters"));
}

TrackstersTestCLUE3D::~TrackstersTestCLUE3D() {}

std::ofstream fileoutCLUE3D("graphsCLUE3D.py");

void TrackstersTestCLUE3D::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
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
    std::cerr << "[TrackstersTestCLUE3D] caught std::exception " << ce.what() << std::endl;
    return;
  }

  for (std::vector<ticl::Trackster>::const_iterator it = tracksters->begin(); it != tracksters->end(); ++it) {
    size_t trkNum = 1;
    const std::vector<unsigned int> &vertices = it->vertices();
    auto &edges = it->edges();

    fileoutCLUE3D << "G" << evtNum << "_" << trkNum << " = nx.Graph()\n";
    fileoutCLUE3D << "G" << evtNum << "_" << trkNum << ".add_nodes_from([";

    for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
      fileoutCLUE3D << *it_vert << ", ";
    }
    fileoutCLUE3D << "])\n";

    if (debug) {
      std::cout << "\nVertice layers: ";
      for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
        auto cluster = layerClusters->at(*it_vert);
        std::vector<std::pair<DetId, float>> clusterDetIds = cluster.hitsAndFractions();
        std::cout << "(" << HGCSiliconDetId(cluster.seed()).layer() << ",";
        std::cout << HGCSiliconDetId(clusterDetIds[0].first).layer() << "), ";
      }
    }

    fileoutCLUE3D << "G" << evtNum << "_" << trkNum << ".add_edges_from([";
    for (auto edge : edges) {
      fileoutCLUE3D << "(" << edge[0] << "," << edge[1] << ")" << ", ";
    }
    fileoutCLUE3D << "])\n\n";

    trkNum++;
  }
}

void TrackstersTestCLUE3D::beginJob() {
  fileoutCLUE3D << "import networkx as nx\n\n";
}

void TrackstersTestCLUE3D::endJob() {}

DEFINE_FWK_MODULE(TrackstersTestCLUE3D);
