#include <string>
#include <iostream>
#include <vector>
#include <map>

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HGCalReco/interface/Trackster.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"
#include "DataFormats/DetId/interface/DetId.h"

#include <Eigen/Core>
#include <Eigen/Dense>

#include "TH1.h"

class Centrality : public edm::EDAnalyzer {
 public:
  explicit Centrality(const edm::ParameterSet&);
  ~Centrality() override;

 private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  edm::EDGetTokenT<std::vector<ticl::Trackster> > tracksters_;
  std::map<int, TH1*> eig_perLayer;
  std::map<int, TH1*> katz_perLayer;
};

Centrality::Centrality(const edm::ParameterSet& iConfig)
{
  tracksters_ = consumes<std::vector<ticl::Trackster>>(edm::InputTag("ticlTrackstersEM"));
  edm::Service<TFileService> fs;
  for (unsigned i = 1; i < 29; ++i) {
    eig_perLayer[i] = fs->make<TH1D>("layer", "layer", 100, 0, 100);
    katz_perLayer[i] = fs->make<TH1D>("layer", "layer", 100, 0, 100);
  }
}

Centrality::~Centrality() {}

void Centrality::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  edm::Handle<std::vector<ticl::Trackster>> tracksterHandle;
  const std::vector<ticl::Trackster> *tracksters;
  try {
    iEvent.getByToken(tracksters_, tracksterHandle);
    tracksters = &(*tracksterHandle);
  } catch (std::exception& ce) {
    std::cerr << "[Centrality] caught std::exception " << ce.what() << std::endl;
    return;
  }

  for (std::vector<ticl::Trackster>::const_iterator it = tracksters->begin(); it != tracksters->end(); ++it) {
    const std::vector<unsigned int> &vertices = it->vertices();
    const auto &edges = it->edges();

    size_t nV = vertices.size();
    Eigen::MatrixXd adj = Eigen::MatrixXd::Zero(nV,nV);

    // Fill adjacency matrix
    for (auto edge : edges) {
      int idx0 = std::find(vertices.begin(),vertices.end(),edge[0]) - vertices.begin();
      int idx1 = std::find(vertices.begin(),vertices.end(),edge[1]) - vertices.begin();
      adj(idx0,idx1) = 1;
      adj(idx1,idx0) = 1;
    }

    // Compute eigenvalues & eigenvectors
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(adj);
    if (eigensolver.info() != Eigen::Success) abort();

    std::cout << "The greatest eigenvalue of A is:\t" << eigensolver.eigenvalues()[nV-1] << std::endl;
    if (eigensolver.eigenvalues()[nV-1] == 0) continue;
    auto c_eig = eigensolver.eigenvectors().col(nV-1);

    // Katz centrality calculation
    const float alpha = 0.9/eigensolver.eigenvalues()[nV-1];
    std::cout << "The attenuation factor alpha is:\t" << alpha << std::fixed << std::endl;

    Eigen::MatrixXd id = Eigen::MatrixXd::Identity(nV,nV);
    Eigen::VectorXd ones = Eigen::VectorXd::Ones(nV);
    Eigen::VectorXd c_katz = ((id-alpha*adj.transpose()).inverse()-id)*ones;

    //std::cout << "Katz centrality:" << std::endl;
    for (size_t i = 1; i < nV+1; ++i) {
      eig_perLayer[i]->Fill(c_eig(i-1));
      katz_perLayer[i]->Fill(c_katz(i-1));
    }
  }
}

void Centrality::beginJob() {}

void Centrality::endJob() {}

DEFINE_FWK_MODULE(Centrality);
