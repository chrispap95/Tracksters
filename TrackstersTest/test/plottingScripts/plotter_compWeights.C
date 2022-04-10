#include "RootStyle.cc"

void plotter_compWeights(){
  set_root_style();
  TString deltas[] = {"1p5", "2", "2p5", "3", "3p5", "4", "4p5"};
  TString methods[] = {"e", "k", "pr"};
  TString methods_desc[] = {"Eigenvector", "Katz", "PageRank"};
  TString weights[] = {"0", "1", "2", "3", "4"};
  TString weights_desc[] = {"unweighted", "max(E1,E2)", "|E2-E1|", "distance^{-1}", "|E2-E1|/distance"};
  TString directed[] = {"","_directed"};
  gStyle->SetPalette(kWaterMelon);

  const size_t nd = sizeof(deltas)/sizeof(deltas[0]);
  const size_t nm = sizeof(methods)/sizeof(methods[0]);
  const size_t nu = sizeof(directed)/sizeof(directed[0]);
  const size_t nl = sizeof(weights)/sizeof(weights[0]);
  const size_t layers = 28;

  TFile* f[nd][nl][nu];
  for (size_t i = 0; i < nd; ++i) {
    for (size_t l = 0; l < nl; ++l) {
      for (size_t j = 0; j < nu; ++j) {
        if (l) f[i][l][j] = TFile::Open("output/hist_CLUE3D_"+deltas[i]+directed[j]+"_weighted"+weights[l]+".root");
        else f[i][l][j] = TFile::Open("output/hist_CLUE3D_"+deltas[i]+directed[j]+".root");
      }
    }
  }

  TCanvas* c1 = new TCanvas("c1","canvas 1",1);
  for (size_t i = 0; i < nd; ++i) {
    for (size_t k = 0; k < nm; ++k) {
      THStack* hs[layers];
      for (size_t j = 0; j < layers; ++j) {
        TString lr = (TString)to_string(j+1);
        TH1D* h[nl][nu];
        for (size_t l = 0; l < nl; ++l) {
          TLegend* lg = new TLegend(0.3, 0.3);
          hs[j] = new THStack();
          lg->SetBorderSize(0);
          h[l][0] = (TH1D*)f[i][l][0]->Get("ana/layer_"+methods[k]+" "+(TString)to_string(j+1));
          h[l][0]->SetTitle("layer "+lr);
          h[l][0]->Rebin();
          hs[j]->Add(h[l][0]);
          lg->AddEntry(h[l][0], weights_desc[l], "l");

          h[l][1] = (TH1D*)f[i][l][1]->Get("ana/layer_"+methods[k]+" "+(TString)to_string(j+1));
          h[l][1]->SetTitle("layer "+lr);
          h[l][1]->SetLineStyle(3);
          h[l][1]->Rebin();
          hs[j]->Add(h[l][1]);
          lg->AddEntry(h[l][1], weights_desc[l]+" + dir.", "l");

          hs[j]->SetTitle("layer "+lr);
          hs[j]->Draw("NOSTACK PLC");
          hs[j]->GetXaxis()->SetTitle("centrality");
          lg->Draw();
          if (j == 0) c1->Print("plots/weighted"+weights[l]+"_"+methods_desc[k]+"_"+deltas[i]+"PerLayer_clue3D.pdf(");
          else if (j == 27) c1->Print("plots/weighted"+weights[l]+"_"+methods_desc[k]+"_"+deltas[i]+"PerLayer_clue3D.pdf)");
          else c1->Print("plots/weighted"+weights[l]+"_"+methods_desc[k]+"_"+deltas[i]+"PerLayer_clue3D.pdf");
        }
      }
    }
  }
}
