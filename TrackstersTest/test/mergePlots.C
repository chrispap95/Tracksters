#include "RootStyle.cc"

void mergePlots(){
  set_root_style();
  TString deltas[] = {"1p5", "2", "2p5", "3", "3p5", "4", "4p5"};
  TString methods[] = {"e", "k", "pr"};
  TString methods_desc[] = {"Eigenvector c.", "Katz c.", "PageRank"};
  int colors[] = {1, 4, 2};

  const size_t nd = sizeof(deltas)/sizeof(deltas[0]);
  const size_t nm = sizeof(methods)/sizeof(methods[0]);
  const size_t layers = 28;

  double dist[nd] = {1.5, 2, 2.5, 3, 3.5, 4, 4.5};
  double err_dist[nd] = {0, 0, 0, 0, 0, 0, 0};

  TFile* f[nd];
  for (size_t i = 0; i < nd; ++i) {
    f[i] = TFile::Open("output/hist_"+deltas[i]+".root");
  }

  TFile* fout = new TFile("output/hist_merged.root", "RECREATE");
  TCanvas* c1 = new TCanvas("c1","canvas 1",1);
  TCanvas* c2 = new TCanvas("c2","canvas 2",1);
  TMultiGraph* mg_mean[layers];
  TMultiGraph* mg_rms[layers];

  for (size_t j = 0; j < layers; ++j) {
    TString lr = (TString)to_string(j+1);
    mg_mean[j] = new TMultiGraph();
    mg_rms[j] = new TMultiGraph();
    TLegend* lg_mean = new TLegend(0.3, 0.3);
    TLegend* lg_rms = new TLegend(0.3, 0.3);
    lg_mean->SetBorderSize(0);
    lg_rms->SetBorderSize(0);
    for (size_t k = 0; k < nm; ++k) {
      double mean[nd];
      double err_mean[nd];
      double rms[nd];
      double err_rms[nd];
      for (size_t i = 0; i < nd; ++i) {
        TH1D* h = (TH1D*)f[i]->Get("ana/layer_"+methods[k]+" "+(TString)to_string(j+1));
        mean[i] = h->GetMean();
        err_mean[i] = h->GetMeanError();
        rms[i] = h->GetRMS();
        err_rms[i] = h->GetRMSError();
      }
      TGraphErrors* gr_mean = new TGraphErrors(nd, dist, mean, err_dist, err_mean);
      gr_mean->SetName("mean_"+methods[k]+" lr"+lr);
      gr_mean->SetTitle("layer "+lr);
      gr_mean->SetLineColor(colors[k]);
      gr_mean->SetMarkerColor(colors[k]);
      gr_mean->Write();
      mg_mean[j]->Add(gr_mean);
      lg_mean->AddEntry(gr_mean, methods_desc[k], "lp");

      TGraphErrors* gr_rms = new TGraphErrors(nd, dist, rms, err_dist, err_rms);
      gr_rms->SetName("rms_"+methods[k]+" lr"+lr);
      gr_rms->SetTitle("layer "+lr);
      gr_rms->SetLineColor(colors[k]);
      gr_rms->SetMarkerColor(colors[k]);
      gr_rms->Write();
      mg_rms[j]->Add(gr_rms);
      lg_rms->AddEntry(gr_rms, methods_desc[k], "lp");
    }
    c1->cd();
    mg_mean[j]->SetTitle("layer "+lr);
    mg_mean[j]->GetXaxis()->SetLimits(1, 5);
    mg_mean[j]->GetYaxis()->SetRangeUser(0, 0.4);
    mg_mean[j]->Draw("AL*");
    mg_mean[j]->GetXaxis()->SetTitle("delta [cm]");
    mg_mean[j]->GetYaxis()->SetTitle("<centrality>");
    lg_mean->Draw();
    if (j == 0) c1->Print("output/meanVsDeltaPerLayer.pdf(");
    else if (j == 27) c1->Print("output/meanVsDeltaPerLayer.pdf)");
    else c1->Print("output/meanVsDeltaPerLayer.pdf");
    c2->cd();
    mg_rms[j]->SetTitle("layer "+lr);
    mg_rms[j]->GetXaxis()->SetLimits(1, 5);
    mg_rms[j]->GetYaxis()->SetRangeUser(0, 0.2);
    mg_rms[j]->Draw("AL*");
    mg_rms[j]->GetXaxis()->SetTitle("delta [cm]");
    mg_rms[j]->GetYaxis()->SetTitle("RMS(centrality)");
    lg_rms->Draw();
    if (j == 0) c2->Print("output/rmsVsDeltaPerLayer.pdf(");
    else if (j == 27) c2->Print("output/rmsVsDeltaPerLayer.pdf)");
    else c2->Print("output/rmsVsDeltaPerLayer.pdf");
  }

  fout->Close();
}
