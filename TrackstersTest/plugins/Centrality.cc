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
#include "TString.h"

class Centrality : public edm::EDAnalyzer {
 public:
  explicit Centrality(const edm::ParameterSet&);
  ~Centrality() override;
  double calcWt(const int, const reco::CaloCluster&, const reco::CaloCluster&, const int);

 private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  edm::EDGetTokenT<std::vector<ticl::Trackster> > tracksters_;
  edm::EDGetTokenT<std::vector<reco::CaloCluster>> layerClusters_;
  std::map<int, TH1*> eig_vsLayer;
  std::map<int, TH1*> katz_vsLayer;
  std::map<int, TH1*> pr_vsLayer;

  bool directed_;
  int weightMode_;
  bool isCLUE3D_;
};

Centrality::Centrality(const edm::ParameterSet& iConfig)
  : directed_(iConfig.getParameter<bool>("directed")),
    weightMode_(iConfig.getParameter<int>("weightMode")),
    isCLUE3D_(iConfig.getParameter<bool>("isCLUE3D"))
{
  if (isCLUE3D_) tracksters_ = consumes<std::vector<ticl::Trackster>>(edm::InputTag("ticlTrackstersCLUE3DHigh"));
  else tracksters_ = consumes<std::vector<ticl::Trackster>>(edm::InputTag("ticlTrackstersMerge"));
  layerClusters_ = consumes<std::vector<reco::CaloCluster>>(edm::InputTag("hgcalLayerClusters"));
  edm::Service<TFileService> fs;
  for (unsigned i = 1; i < 29; ++i) {
    TString title_layer = "layer "+(TString)std::to_string(i);
    eig_vsLayer[i] = fs->make<TH1D>("layer_e "+(TString)std::to_string(i), title_layer, 100, 0, 0.5);
    katz_vsLayer[i] = fs->make<TH1D>("layer_k "+(TString)std::to_string(i), title_layer, 100, 0, 0.5);
    pr_vsLayer[i] = fs->make<TH1D>("layer_pr "+(TString)std::to_string(i), title_layer, 100, 0, 0.5);
  }
}

Centrality::~Centrality() {}

double Centrality::calcWt(const int mode, const reco::CaloCluster& cluster1, const reco::CaloCluster& cluster2, const int n = 1) {
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

void Centrality::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
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
    std::cerr << "[Centrality] caught std::exception " << ce.what() << std::endl;
    return;
  }

  for (std::vector<ticl::Trackster>::const_iterator it = tracksters->begin(); it != tracksters->end(); ++it) {
    const std::vector<unsigned int> &vertices = it->vertices();
    const auto &edges = it->edges();

    // Define commonly used objects
    size_t nV = vertices.size();
    Eigen::MatrixXd adj = Eigen::MatrixXd::Zero(nV,nV);
    Eigen::MatrixXd id = Eigen::MatrixXd::Identity(nV,nV);
    Eigen::VectorXd v_ones = Eigen::VectorXd::Ones(nV);
    Eigen::MatrixXd m_ones = Eigen::MatrixXd::Ones(nV,nV);

    // Fill adjacency matrix
    for (auto edge : edges) {
      unsigned idx0 = std::find(vertices.begin(),vertices.end(),edge[0]) - vertices.begin();
      unsigned idx1 = std::find(vertices.begin(),vertices.end(),edge[1]) - vertices.begin();
      if (idx0 == nV || idx1 == nV) {
        std::cout << "Warning: vertice is out of bounds.\n";
        continue;
      }
      const auto cluster1 = layerClusters->at(edge[0]);
      const auto cluster2 = layerClusters->at(edge[1]);
      double weight = Centrality::calcWt(weightMode_, cluster1, cluster2, 1);
      adj(idx0,idx1) = weight;
      if (!directed_) adj(idx1,idx0) = weight;
    }

    // Fill list of layer numbers
    unsigned layers[nV];
    unsigned j = 0;
    for (std::vector<unsigned int>::const_iterator it_vert = vertices.begin(); it_vert != vertices.end(); it_vert++){
      auto cluster = layerClusters->at(*it_vert);
      layers[j] = HGCSiliconDetId(cluster.seed()).layer();
      j++;
    }

    // Compute eigenvalues & eigenvectors
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(adj);
    if (eigensolver.info() != Eigen::Success) abort();
    if (eigensolver.eigenvalues()[nV-1] == 0) continue;
    auto c_eig = eigensolver.eigenvectors().col(nV-1);

    // Katz centrality calculation
    const float alpha = 0.9/eigensolver.eigenvalues()[nV-1]; // has to be smaller than 1/(largest eigenvalue)
    Eigen::VectorXd c_katz = ((id-alpha*adj.transpose()).inverse()-id)*v_ones;

    // PageRank calculation
    const float df = 0.85;
    Eigen::MatrixXd m_pr = df*adj+(1-df)*m_ones/nV;
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver_pr(m_pr);
    if (eigensolver_pr.info() != Eigen::Success) abort();
    if (eigensolver_pr.eigenvalues()[nV-1] == 0) continue;
    auto c_pr = eigensolver_pr.eigenvectors().col(nV-1);

    // Calculate normalization factors
    double norm_eig = 0;
    double norm_katz = 0;
    double norm_pr = 0;
    for (size_t i = 0; i < nV; ++i) {
      norm_eig += pow(c_eig(i),2);
      norm_katz += pow(c_katz(i),2);
      norm_pr += pow(c_pr(i),2);
    }

    // Fill the histograms
    for (size_t i = 0; i < nV; ++i) {
      unsigned layer = layers[i];
      eig_vsLayer[layer]->Fill(c_eig(i)/sqrt(norm_eig));
      katz_vsLayer[layer]->Fill(c_katz(i)/sqrt(norm_katz));
      pr_vsLayer[layer]->Fill(c_pr(i)/sqrt(norm_pr));
    }
  }
}

void Centrality::beginJob() {}

void Centrality::endJob() {}

DEFINE_FWK_MODULE(Centrality);
