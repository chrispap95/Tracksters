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

class TrackstersTestCA : public edm::EDAnalyzer {
 public:
  explicit TrackstersTestCA(const edm::ParameterSet&);
  ~TrackstersTestCA() override;

 private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  edm::EDGetTokenT<std::vector<ticl::Trackster> > tracksters_;
  edm::EDGetTokenT<std::vector<reco::CaloCluster>> layerClusters_;

};

TrackstersTestCA::TrackstersTestCA(const edm::ParameterSet& iConfig) {
  tracksters_ = consumes<std::vector<ticl::Trackster>>(edm::InputTag("ticlTrackstersMerge"));
  layerClusters_ = consumes<std::vector<reco::CaloCluster>>(edm::InputTag("hgcalLayerClusters"));
}

TrackstersTestCA::~TrackstersTestCA() {}

std::ofstream fileoutCA("graphsCA.py");

void TrackstersTestCA::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
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
    std::cerr << "[TrackstersTestCA] caught std::exception " << ce.what() << std::endl;
    return;
  }

  for (std::vector<ticl::Trackster>::const_iterator it = tracksters->begin(); it != tracksters->end(); ++it) {
    size_t trkNum = 1;
    const std::vector<unsigned int> &vertices = it->vertices();
    auto &edges = it->edges();

    fileoutCA << "G" << evtNum << "_" << trkNum << " = nx.Graph()\n";
    fileoutCA << "G" << evtNum << "_" << trkNum << ".add_nodes_from([";

    for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
      fileoutCA << *it_vert << ", ";
    }
    fileoutCA << "])\n";

    if (debug) {
      std::cout << "\nVertice layers: ";
      for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++) {
        auto cluster = layerClusters->at(*it_vert);
        std::vector<std::pair<DetId, float>> clusterDetIds = cluster.hitsAndFractions();
        std::cout << "(" << HGCSiliconDetId(cluster.seed()).layer() << ",";
        std::cout << HGCSiliconDetId(clusterDetIds[0].first).layer() << "), ";
      }
    }

    fileoutCA << "G" << evtNum << "_" << trkNum << ".add_edges_from([";
    for (auto edge : edges) {
      fileoutCA << "(" << edge[0] << "," << edge[1] << ")" << ", ";
    }
    fileoutCA << "])\n\n";

    trkNum++;
  }
}

void TrackstersTestCA::beginJob() {
  fileoutCA << "import networkx as nx\n\n";
}

void TrackstersTestCA::endJob() {}

DEFINE_FWK_MODULE(TrackstersTestCA);
