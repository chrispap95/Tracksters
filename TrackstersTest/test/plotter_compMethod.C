#include "RootStyle.cc"

void plotter_compMethod(){
  set_root_style();
  TString deltas[] = {"1p5", "2", "2p5", "3", "3p5", "4", "4p5"};
  TString methods[] = {"e", "k", "pr"};
  TString methods_desc[] = {"Eigenvector", "Katz", "PageRank"};
  //gStyle->SetPalette(112);

  const size_t nd = sizeof(deltas)/sizeof(deltas[0]);
  const size_t nm = sizeof(methods)/sizeof(methods[0]);
  const size_t layers = 28;


  TFile* f[nd];
  for (size_t i = 0; i < nd; ++i) {
    f[i] = TFile::Open("output/hist_"+deltas[i]+".root");
  }

  TCanvas* c1 = new TCanvas("c1","canvas 1",1);
  for (size_t i = 0; i < nd; ++i) {
    THStack* hs[layers];
    for (size_t j = 0; j < layers; ++j) {
      TString lr = (TString)to_string(j+1);
      TH1D* h[nm];
      TLegend* lg = new TLegend(0.3, 0.3);
      hs[j] = new THStack();
      lg->SetBorderSize(0);
      for (size_t k = 0; k < nm; ++k) {
        h[k] = (TH1D*)f[i]->Get("ana/layer_"+methods[k]+" "+(TString)to_string(j+1));
        h[k]->SetTitle("layer "+lr);
        h[k]->Rebin();
        hs[j]->Add(h[k]);
        lg->AddEntry(h[k], methods_desc[k], "l");
      }
      hs[j]->SetTitle("layer "+lr);
      hs[j]->Draw("NOSTACK PLC");
      hs[j]->GetXaxis()->SetTitle("centrality");
      lg->Draw();
      if (j == 0) c1->Print("output/"+deltas[i]+"PerLayer.pdf(");
      else if (j == 27) c1->Print("output/"+deltas[i]+"PerLayer.pdf)");
      else c1->Print("output/"+deltas[i]+"PerLayer.pdf");
    }
  }
}
