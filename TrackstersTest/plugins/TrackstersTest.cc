#include <string>
#include <iostream>
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

class TrackstersTest : public edm::EDAnalyzer {
 public:
  explicit TrackstersTest(const edm::ParameterSet&);
  ~TrackstersTest() override;

 private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  edm::EDGetTokenT<std::vector<ticl::Trackster> > tracksters_;
  edm::EDGetTokenT<std::vector<reco::CaloCluster>> layerClusters_;

};

TrackstersTest::TrackstersTest(const edm::ParameterSet& iConfig) {
  tracksters_ = consumes<std::vector<ticl::Trackster>>(edm::InputTag("ticlTrackstersEM"));
  layerClusters_ = consumes<std::vector<reco::CaloCluster>>(edm::InputTag("hgcalLayerClusters"));
}

TrackstersTest::~TrackstersTest() {}

void TrackstersTest::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
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
    std::cerr << "[TrackstersTest] caught std::exception " << ce.what() << std::endl;
    return;
  }

  for (std::vector<ticl::Trackster>::const_iterator it = tracksters->begin(); it != tracksters->end(); ++it) {
    const std::vector<unsigned int> &vertices = it->vertices();
    const auto &eigenvalues = it->eigenvalues();
    auto &edges = it->edges();

    std::cout << "Trackster\nVertices: ";
    for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++){
      std::cout << *it_vert << ", ";
    }
    std::cout << "\nVertice layers: ";
    for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++){
      auto cluster = layerClusters->at(*it_vert);
      std::vector<std::pair<DetId, float>> clusterDetIds = cluster.hitsAndFractions();
      //std::cout << "(" << HGCSiliconDetId(cluster.seed()).layer() << ",";
      //std::cout << HGCSiliconDetId(clusterDetIds[0].first).layer() << "), ";
    }

    std::unordered_set<unsigned int> targetVert;
    int nRedundant = 0;
    std::cout << "\nEdges: ";
    for (auto edge : edges) {
      std::cout << "(" << edge[0] << "," << edge[1] << ")" << ", ";
      if (targetVert.find(edge[1]) == targetVert.end()) {
        targetVert.insert(edge[1]);
      } else {
        nRedundant++;
      }
    }
    std::cout << "\nFound " << nRedundant << " redundant connections out of a total of " << edges.size() << std::endl;
    std::cout << "\nPruned edges: ";
    std::unordered_set<unsigned int> targetVert2;
    for (auto edge : edges) {
      if (targetVert2.find(edge[1]) == targetVert2.end()) {
        targetVert2.insert(edge[1]);
        std::cout << "(" << edge[0] << "," << edge[1] << ")" << ", ";
      }
    }

    std::cout << "\nEigenvalues: " << eigenvalues[0] << ", " << eigenvalues[1] << ", " << eigenvalues[2];
    std::cout << std::endl;
  }
}

void TrackstersTest::beginJob() {}

void TrackstersTest::endJob() {}

DEFINE_FWK_MODULE(TrackstersTest);
