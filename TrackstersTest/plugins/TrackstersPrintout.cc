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

TrackstersPrintout::TrackstersPrintout(const edm::ParameterSet& iConfig) :
  isCLUE3D_(iConfig.getParameter<bool>("isCLUE3D")) {
  std::string trackstersTag = isCLUE3D_ ? "ticlTrackstersCLUE3DHigh" : "ticlTrackstersMerge";
  tracksters_ = consumes<std::vector<ticl::Trackster>>(edm::InputTag(trackstersTag));
  layerClusters_ = consumes<std::vector<reco::CaloCluster>>(edm::InputTag("hgcalLayerClusters"));
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

    Eigen::MatrixXd adj[5];

    for (size_t iW = 0; iW < 5; ++iW) {
      // Fill adjacency matrix
      adj[iW] = ticl::adjacency(iW, false, vertices, edges, layerClusters);
      std::cout << std::fixed << std::setprecision(2) << adj[iW] << "\n\n";
      std::cout << std::fixed << std::setprecision(2) << ticl::centralityEigenvector(adj[iW]) << "\n\n\n";
    }

    trkNum++;
  }
}

void TrackstersPrintout::beginJob() {
}

void TrackstersPrintout::endJob() {}

DEFINE_FWK_MODULE(TrackstersPrintout);
