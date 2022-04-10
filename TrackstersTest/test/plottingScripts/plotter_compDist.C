/*******************************************************************
**
**  Comparison plots: centrality distributions with different deltas
**  on each plot. Centrality methods are shown separately.
**
*******************************************************************/

#include "RootStyle.cc"

void plotter_compDist(){
  set_root_style();
  TString deltas[] = {"1p5", "2p5", "3p5", "4p5"};
  TString dist[] = {"1.5", "2.5", "3.5", "4.5"};
  TString methods[] = {"e", "k", "pr"};
  TString methods_desc[] = {"Eigenvector", "Katz", "PageRank"};
  //gStyle->SetPalette(112);

  const size_t nd = sizeof(deltas)/sizeof(deltas[0]);
  const size_t nm = sizeof(methods)/sizeof(methods[0]);
  const size_t layers = 28;


  TFile* f[nd];
  for (size_t i = 0; i < nd; ++i) {
    f[i] = TFile::Open("output/hist_clue3D_"+deltas[i]+".root");
  }

  TCanvas* c1 = new TCanvas("c1","canvas 1",1);\
  for (size_t k = 0; k < nm; ++k) {
    THStack* hs[layers];
    for (size_t j = 0; j < layers; ++j) {
      TString lr = (TString)to_string(j+1);
      TH1D* h[nd];
      TLegend* lg = new TLegend(0.3, 0.3);
      hs[j] = new THStack();
      lg->SetBorderSize(0);
      for (size_t i = 0; i < nd; ++i) {
        h[i] = (TH1D*)f[i]->Get("ana/layer_"+methods[k]+" "+(TString)to_string(j+1));
        h[i]->SetTitle("layer "+lr);
        h[i]->Rebin();
        hs[j]->Add(h[i]);
        lg->AddEntry(h[i], "#delta = "+dist[i]+" cm", "l");
      }
      hs[j]->SetTitle("layer "+lr);
      hs[j]->Draw("NOSTACK PLC");
      hs[j]->GetXaxis()->SetTitle(methods_desc[k]+" centrality");
      lg->Draw();
      if (j == 0) c1->Print("output/"+methods_desc[k]+"PerLayer_clue3D.pdf(");
      else if (j == 27) c1->Print("output/"+methods_desc[k]+"PerLayer_clue3D.pdf)");
      else c1->Print("output/"+methods_desc[k]+"PerLayer_clue3D.pdf");
    }
  }
}
