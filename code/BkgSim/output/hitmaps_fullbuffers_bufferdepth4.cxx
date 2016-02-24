{
//=========Macro generated from canvas: canvas/canvas
//=========  (Wed Feb 24 10:48:53 2016) by ROOT version5.34/05
   TCanvas *canvas = new TCanvas("canvas", "canvas",0,0,800,600);
   gStyle->SetOptFit(1);
   canvas->SetHighLightColor(2);
   canvas->Range(-2227.273,-1928.571,2318.182,1967.532);
   canvas->SetFillColor(0);
   canvas->SetBorderMode(0);
   canvas->SetBorderSize(2);
   canvas->SetTickx(1);
   canvas->SetTicky(1);
   canvas->SetLeftMargin(0.16);
   canvas->SetRightMargin(0.18);
   canvas->SetTopMargin(0.12);
   canvas->SetFrameBorderMode(0);
   canvas->SetFrameBorderMode(0);
   
   TH2D *histo = new TH2D("histo","Hit maps of cells with full buffer for subdetector EcalEndcap",70,-1500,1500,70,-1500,1500);
   histo->SetBinContent(2123,0.125);
   histo->SetBinContent(2196,0.25);
   histo->SetBinContent(2345,0.125);
   histo->SetBinContent(2407,0.125);
   histo->SetBinContent(2417,0.125);
   histo->SetBinContent(2418,0.125);
   histo->SetBinContent(2840,0.125);
   histo->SetEntries(8);
   histo->SetContour(20);
   histo->SetContourLevel(0,0);
   histo->SetContourLevel(1,0.0125);
   histo->SetContourLevel(2,0.025);
   histo->SetContourLevel(3,0.0375);
   histo->SetContourLevel(4,0.05);
   histo->SetContourLevel(5,0.0625);
   histo->SetContourLevel(6,0.075);
   histo->SetContourLevel(7,0.0875);
   histo->SetContourLevel(8,0.1);
   histo->SetContourLevel(9,0.1125);
   histo->SetContourLevel(10,0.125);
   histo->SetContourLevel(11,0.1375);
   histo->SetContourLevel(12,0.15);
   histo->SetContourLevel(13,0.1625);
   histo->SetContourLevel(14,0.175);
   histo->SetContourLevel(15,0.1875);
   histo->SetContourLevel(16,0.2);
   histo->SetContourLevel(17,0.2125);
   histo->SetContourLevel(18,0.225);
   histo->SetContourLevel(19,0.2375);
   
   TPaletteAxis *palette = new TPaletteAxis(1522.727,-1500,1727.273,1500,histo);
palette->SetLabelColor(1);
palette->SetLabelFont(22);
palette->SetLabelOffset(0.005);
palette->SetLabelSize(0.05);
palette->SetTitleOffset(1.3);
palette->SetTitleSize(0.05);

   Int_t ci;   // for color index setting
   ci = TColor::GetColor("#fffffb");
   palette->SetFillColor(ci);
   palette->SetFillStyle(1001);
   histo->GetListOfFunctions()->Add(palette,"br");
   
   TPaveStats *ptstats = new TPaveStats(0.78,0.755,0.98,0.995,"brNDC");
   ptstats->SetName("stats");
   ptstats->SetBorderSize(2);
   ptstats->SetFillColor(0);
   ptstats->SetTextAlign(12);
   TText *text = ptstats->AddText("histo");
   text->SetTextSize(0.0368);
   text = ptstats->AddText("Entries = 8      ");
   text = ptstats->AddText("Mean x =     49");
   text = ptstats->AddText("Mean y = -131.7");
   text = ptstats->AddText("RMS x =  162.3");
   text = ptstats->AddText("RMS y =  129.5");
   ptstats->SetOptStat(1111);
   ptstats->SetOptFit(1111);
   ptstats->Draw();
   histo->GetListOfFunctions()->Add(ptstats);
   ptstats->SetParent(histo);
   histo->SetLineWidth(2);
   histo->SetMarkerStyle(20);
   histo->SetMarkerSize(1.2);
   histo->GetXaxis()->SetTitle("x [mm]");
   histo->GetXaxis()->SetLabelFont(22);
   histo->GetXaxis()->SetLabelSize(0.05);
   histo->GetXaxis()->SetTitleSize(0.05);
   histo->GetXaxis()->SetTitleOffset(1.05);
   histo->GetXaxis()->SetTitleFont(22);
   histo->GetYaxis()->SetTitle(" y [mm]");
   histo->GetYaxis()->SetLabelFont(22);
   histo->GetYaxis()->SetLabelSize(0.05);
   histo->GetYaxis()->SetTitleSize(0.05);
   histo->GetYaxis()->SetTitleOffset(1.4);
   histo->GetYaxis()->SetTitleFont(22);
   histo->GetZaxis()->SetTitle("# of hits");
   histo->GetZaxis()->SetLabelFont(22);
   histo->GetZaxis()->SetLabelSize(0.05);
   histo->GetZaxis()->SetTitleSize(0.05);
   histo->GetZaxis()->SetTitleOffset(1.3);
   histo->GetZaxis()->SetTitleFont(22);
   histo->Draw("colz");
   
   TPaveText *pt = new TPaveText(0.01,0.9425525,0.71,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(2);
   text = pt->AddText("Hit maps of cells with full buffer for subdetector EcalEndcap");
   pt->Draw();
   canvas->Modified();
   canvas->cd();
   canvas->SetSelected(canvas);
}
