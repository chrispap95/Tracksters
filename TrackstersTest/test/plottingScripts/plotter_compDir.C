#include "RootStyle.cc"

void plotter_compDir(){
  set_root_style();
  TString deltas[] = {"1p5", "2", "2p5", "3", "3p5", "4", "4p5"};
  TString methods[] = {"e", "k", "pr"};
  TString methods_desc[] = {"Eigenvector", "Katz", "PageRank"};
  TString directed[] = {"","_directed"};
  //gStyle->SetPalette(112);

  const size_t nd = sizeof(deltas)/sizeof(deltas[0]);
  const size_t nm = sizeof(methods)/sizeof(methods[0]);
  const size_t nu = sizeof(directed)/sizeof(directed[0]);
  const size_t layers = 28;

  TFile* f[nd][nu];
  for (size_t i = 0; i < nd; ++i) {
    for (size_t j = 0; j < nu; ++j) {
      f[i][j] = TFile::Open("output/hist_CLUE3D_"+deltas[i]+directed[j]+".root");
    }
  }

  TCanvas* c1 = new TCanvas("c1","canvas 1",1);
  for (size_t i = 0; i < nd; ++i) {
    THStack* hs[layers];
    for (size_t j = 0; j < layers; ++j) {
      TString lr = (TString)to_string(j+1);
      TH1D* h[nm][nu];
      TLegend* lg = new TLegend(0.3, 0.3);
      hs[j] = new THStack();
      lg->SetBorderSize(0);
      for (size_t k = 0; k < nm; ++k) {
        h[k][0] = (TH1D*)f[i][0]->Get("ana/layer_"+methods[k]+" "+(TString)to_string(j+1));
        h[k][0]->SetTitle("layer "+lr);
        h[k][0]->Rebin();
        hs[j]->Add(h[k][0]);
        lg->AddEntry(h[k][0], methods_desc[k], "l");

        h[k][1] = (TH1D*)f[i][1]->Get("ana/layer_"+methods[k]+" "+(TString)to_string(j+1));
        h[k][1]->SetTitle("layer "+lr);
        h[k][1]->SetLineStyle(3);
        h[k][1]->Rebin();
        hs[j]->Add(h[k][1]);
        lg->AddEntry(h[k][1], methods_desc[k]+" + dir.", "l");
      }
      hs[j]->SetTitle("layer "+lr);
      hs[j]->Draw("NOSTACK PLC");
      hs[j]->GetXaxis()->SetTitle("centrality");
      lg->Draw();
      if (j == 0) c1->Print("output/dirVSundir_"+deltas[i]+"PerLayer_clue3D.pdf(");
      else if (j == 27) c1->Print("output/dirVSundir_"+deltas[i]+"PerLayer_clue3D.pdf)");
      else c1->Print("output/dirVSundir_"+deltas[i]+"PerLayer_clue3D.pdf");
    }
  }
}
