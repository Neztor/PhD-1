{
//=========Macro generated from canvas: canvas/canvas
//=========  (Tue Feb 23 15:23:47 2016) by ROOT version5.34/05
   TCanvas *canvas = new TCanvas("canvas", "canvas",0,0,800,600);
   canvas->SetHighLightColor(2);
   canvas->Range(-4375,-265.1651,4375,2386.485);
   canvas->SetFillColor(0);
   canvas->SetBorderMode(0);
   canvas->SetBorderSize(2);
   canvas->SetFrameBorderMode(0);
   canvas->SetFrameBorderMode(0);
   
   TH2D *histo2 = new TH2D("histo2","10ns <= Time < 20ns, Hit maps of particle origins for those particles hitting subdetector EcalEndcap",702,-3500,3500,106,0,2121.32);
   histo2->SetBinContent(1033,1);
   histo2->SetBinContent(1037,2);
   histo2->SetBinContent(1038,1);
   histo2->SetBinContent(1039,1);
   histo2->SetBinContent(1041,1);
   histo2->SetBinContent(1045,1);
   histo2->SetBinContent(1047,1);
   histo2->SetBinContent(1049,1);
   histo2->SetBinContent(1050,1);
   histo2->SetBinContent(1051,1);
   histo2->SetBinContent(1052,31);
   histo2->SetBinContent(1055,1);
   histo2->SetBinContent(1056,118);
   histo2->SetBinContent(1061,1);
   histo2->SetBinContent(1062,5);
   histo2->SetBinContent(1063,4);
   histo2->SetBinContent(1064,1);
   histo2->SetBinContent(1065,1);
   histo2->SetBinContent(1069,1);
   histo2->SetBinContent(1070,1);
   histo2->SetBinContent(1074,1);
   histo2->SetBinContent(1429,1);
   histo2->SetBinContent(1703,1);
   histo2->SetBinContent(1712,3);
   histo2->SetBinContent(1725,1);
   histo2->SetBinContent(1729,2);
   histo2->SetBinContent(1734,1);
   histo2->SetBinContent(1756,1);
   histo2->SetBinContent(1782,1);
   histo2->SetBinContent(1790,1);
   histo2->SetBinContent(1797,1);
   histo2->SetBinContent(1801,1);
   histo2->SetBinContent(1803,1);
   histo2->SetBinContent(2367,1);
   histo2->SetBinContent(2375,2);
   histo2->SetBinContent(2381,1);
   histo2->SetBinContent(2385,2);
   histo2->SetBinContent(2387,1);
   histo2->SetBinContent(2551,1);
   histo2->SetBinContent(3029,1);
   histo2->SetBinContent(3038,1);
   histo2->SetBinContent(3161,1);
   histo2->SetBinContent(3292,2);
   histo2->SetBinContent(3313,23);
   histo2->SetBinContent(3324,2);
   histo2->SetBinContent(5192,8);
   histo2->SetBinContent(6531,1);
   histo2->SetBinContent(9396,1);
   histo2->SetBinContent(10072,1);
   histo2->SetBinContent(10776,2);
   histo2->SetBinContent(13592,3);
   histo2->SetBinContent(14584,3);
   histo2->SetBinContent(19465,1);
   histo2->SetBinContent(19913,2);
   histo2->SetBinContent(22327,1);
   histo2->SetBinContent(25606,2);
   histo2->SetBinContent(26250,1);
   histo2->SetBinContent(31458,1);
   histo2->SetBinContent(33336,1);
   histo2->SetBinContent(38519,2);
   histo2->SetBinContent(40328,1);
   histo2->SetEntries(260);
   histo2->SetContour(20);
   histo2->SetContourLevel(0,0);
   histo2->SetContourLevel(1,5.9);
   histo2->SetContourLevel(2,11.8);
   histo2->SetContourLevel(3,17.7);
   histo2->SetContourLevel(4,23.6);
   histo2->SetContourLevel(5,29.5);
   histo2->SetContourLevel(6,35.4);
   histo2->SetContourLevel(7,41.3);
   histo2->SetContourLevel(8,47.2);
   histo2->SetContourLevel(9,53.1);
   histo2->SetContourLevel(10,59);
   histo2->SetContourLevel(11,64.9);
   histo2->SetContourLevel(12,70.8);
   histo2->SetContourLevel(13,76.7);
   histo2->SetContourLevel(14,82.6);
   histo2->SetContourLevel(15,88.5);
   histo2->SetContourLevel(16,94.4);
   histo2->SetContourLevel(17,100.3);
   histo2->SetContourLevel(18,106.2);
   histo2->SetContourLevel(19,112.1);
   
   TPaletteAxis *palette = new TPaletteAxis(3543.75,0,3937.5,2121.32,histo2);
palette->SetLabelColor(1);
palette->SetLabelFont(42);
palette->SetLabelOffset(0.005);
palette->SetLabelSize(0.035);
palette->SetTitleOffset(1);
palette->SetTitleSize(0.035);
   palette->SetFillColor(100);
   palette->SetFillStyle(1001);
   histo2->GetListOfFunctions()->Add(palette,"br");
   
   TPaveStats *ptstats = new TPaveStats(0.78,0.575,0.98,0.935,"brNDC");
   ptstats->SetName("stats");
   ptstats->SetBorderSize(1);
   ptstats->SetFillColor(0);
   ptstats->SetTextAlign(12);
   ptstats->SetTextFont(42);
   TText *text = ptstats->AddText("histo2");
   text->SetTextSize(0.0368);
   text = ptstats->AddText("Entries = 260    ");
   text = ptstats->AddText("Mean x =  39.74");
   text = ptstats->AddText("Mean y =  69.84");
   text = ptstats->AddText("RMS x =  699.6");
   text = ptstats->AddText("RMS y =  178.3");
   text = ptstats->AddText("       0|      0|      0
");
   text = ptstats->AddText("       0|    260|      0
");
   text = ptstats->AddText("       0|      0|      0
");
   ptstats->SetOptStat(11111);
   ptstats->SetOptFit(0);
   ptstats->Draw();
   histo2->GetListOfFunctions()->Add(ptstats);
   ptstats->SetParent(histo2);

   Int_t ci;   // for color index setting
   ci = TColor::GetColor("#000099");
   histo2->SetLineColor(ci);
   histo2->GetXaxis()->SetTitle("z [mm]");
   histo2->GetXaxis()->SetLabelFont(42);
   histo2->GetXaxis()->SetLabelSize(0.035);
   histo2->GetXaxis()->SetTitleSize(0.035);
   histo2->GetXaxis()->SetTitleFont(42);
   histo2->GetYaxis()->SetTitle(" r [mm]");
   histo2->GetYaxis()->SetLabelFont(42);
   histo2->GetYaxis()->SetLabelSize(0.035);
   histo2->GetYaxis()->SetTitleSize(0.035);
   histo2->GetYaxis()->SetTitleFont(42);
   histo2->GetZaxis()->SetLabelFont(42);
   histo2->GetZaxis()->SetLabelSize(0.035);
   histo2->GetZaxis()->SetTitleSize(0.035);
   histo2->GetZaxis()->SetTitleFont(42);
   histo2->Draw("colz");
   
   TPaveText *pt = new TPaveText(0.15,0.9341608,0.85,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextFont(42);
   text = pt->AddText("10ns <= Time < 20ns, Hit maps of particle origins for those particles hitting subdetector EcalEndcap");
   pt->Draw();
   canvas->Modified();
   canvas->cd();
   canvas->SetSelected(canvas);
}
