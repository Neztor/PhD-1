#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TFrame.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TLine.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "Style.h"

#include "UsefulFunctions.h"
#include "Style.h"

void Plot_everything(std::vector< std::string > inputfilenames, std::string layer, std::string outputfilename, bool barrel);
void Plot_Comparison_histos(std::vector< std::string > inputfilenames, std::string histoname, std::string x_title, std::string y_title);

int main(int const argc, char const * const * const argv){
  UsePhDStyle();

	std::vector< std::string > inputfilenames;
	std::string outputfilename;
	std::string subdetectorshape;
  bool barrel = false;

	if (argc < 5) {
		std::cerr << "Please provide the names of the input and output files: " << std::endl;
		std::cerr << "./CompareOccupancies -i file1.root file2.root -o output" << std::endl;
		exit(2);
	}
	for (int i = 1; i < argc; i++) {
		if (argv[i] == std::string("-i")){
			if (argv[i + 1] != NULL 
					&& argv[i + 1] != std::string("-s")
          && argv[i + 1] != std::string("-o")) {
				int j = 1;
				do {
					inputfilenames.push_back(argv[i + j]);
					++j;
				} while (argv[i + j] != NULL 
					  && argv[i + j] != std::string("-s")
            && argv[i + j] != std::string("-o") );
			} else {
				std::cerr << "You didn't give an argument for the inputfile(s)!" << std::endl;
				exit(2);
			}
		}
		if (argv[i] == std::string("-o")) {
			if (argv[i + 1] != NULL 
					&& argv[i + 1] != std::string("-s")
					&& argv[i + 1] != std::string("-i")) {
				outputfilename = argv[i + 1];
			} else {
				std::cerr << "You didn't give an argument for the outputfilename!"
					<< std::endl;
				exit(2);
			}
		}
    if (argv[i] == std::string("-s")) {
			if (argv[i + 1] != NULL 
					&& argv[i + 1] != std::string("-o")
					&& argv[i + 1] != std::string("-i")) {
				subdetectorshape = argv[i + 1];
			} else {
				std::cerr << "You didn't give an argument for the shape of the subdetector!"
					<< std::endl;
				exit(2);
			}
		}

	}
  if(subdetectorshape.compare("barrel") == 0){
    barrel = true;
  }
  else if(subdetectorshape.compare("endcap") == 0){
    barrel = false;
  }
  else{
    std::cerr << "Subdetector shape not recognized!" << std::endl;
    exit(-2);
  }

  Plot_everything( inputfilenames, "All_layers", outputfilename, barrel);
  Plot_everything( inputfilenames, "Layer_0",    outputfilename, barrel);
  Plot_everything( inputfilenames, "Layer_1",    outputfilename, barrel);
  Plot_everything( inputfilenames, "Layer_2",    outputfilename, barrel);
  Plot_everything( inputfilenames, "Layer_3",    outputfilename, barrel);
  Plot_everything( inputfilenames, "Layer_4",    outputfilename, barrel);

	return 0;
}

void Plot_everything(std::vector< std::string > inputfilenames, std::string layer, std::string outputfilename, bool barrel) {
  TCanvas* canvas = new TCanvas();
  canvas->SetLogy();
  canvas->SetGridx();
  canvas->SetGridy();
  gStyle->SetOptStat(0);

  std::string xtitle1, xtitle2;
  if(barrel){
    xtitle1 = "#phi [rad]";
    xtitle2 = "z [mm]";
  }
  else if (!barrel){
    xtitle1 = "x [mm]";
    xtitle2 = "y [mm]";
  }
 
  Plot_Comparison_histos( inputfilenames, layer+"_losthits1", xtitle1, "Average number of hits lost" );
  canvas->Update();
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_losthits1_"+outputfilename+".pdf").c_str() );
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_losthits1_"+outputfilename+".cxx").c_str() );
  
  Plot_Comparison_histos( inputfilenames, layer+"_losthits2", xtitle2, "Average number of hits lost" );
  canvas->Update();
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_losthits2_"+outputfilename+".pdf").c_str() );
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_losthits2_"+outputfilename+".cxx").c_str() );

  Plot_Comparison_histos( inputfilenames, layer+"_deadcells1", xtitle1, "Number of dead cells" );
  canvas->Update();
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_deadcells1_"+outputfilename+".pdf").c_str() );
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_deadcells1_"+outputfilename+".cxx").c_str() );

  Plot_Comparison_histos( inputfilenames, layer+"_deadcells2", xtitle2, "Number of dead cells" );
  canvas->Update();
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_deadcells2_"+outputfilename+".pdf").c_str() );
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_deadcells2_"+outputfilename+".cxx").c_str() );
  
  if (layer.compare("All_layers") == 0 ){
  Plot_Comparison_histos( inputfilenames, layer+"_normocc1", xtitle1, "Average number of hits per cell" );
  canvas->Update();
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_occupancy1_"+outputfilename+".pdf").c_str() );
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_occupancy1_"+outputfilename+".cxx").c_str() );

  Plot_Comparison_histos( inputfilenames, layer+"_normocc2", xtitle2, "Average number of hits per cell" );
  canvas->Update();
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_occupancy2_"+outputfilename+".pdf").c_str() );
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_occupancy2_"+outputfilename+".cxx").c_str() );
  }
  else {
  Plot_Comparison_histos( inputfilenames, layer+"_occupancy1", xtitle1, "Average number of hits per cell" );
  canvas->Update();
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_occupancy1_"+outputfilename+".pdf").c_str() );
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_occupancy1_"+outputfilename+".cxx").c_str() );

  Plot_Comparison_histos( inputfilenames, layer+"_occupancy2", xtitle2, "Average number of hits per cell" );
  canvas->Update();
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_occupancy2_"+outputfilename+".pdf").c_str() );
  canvas->Print( ("output/Occupancy_Phi_Comparison_"+layer+"_occupancy2_"+outputfilename+".cxx").c_str() );
  }

  //Plot_Comparison_histos( inputfilenames, layer, "Number of hits per cell", "Number of cells" );
  //canvas->Update();
  //canvas->Print( ("output/Occupancy_Comparison_"+layer+"_"+outputfilename+".pdf").c_str() );
  //canvas->Print( ("output/Occupancy_Comparison_"+layer+"_"+outputfilename+".cxx").c_str() );

  //Plot_Comparison_histos( inputfilenames, layer+"_losthits", "Assumed buffer depth", "Number of hits lost" );
  //canvas->Update();
  //canvas->Print( ("output/Occupancy_Comparison_"+layer+"_losthits_"+outputfilename+".pdf").c_str() );
  //canvas->Print( ("output/Occupancy_Comparison_"+layer+"_losthits_"+outputfilename+".cxx").c_str() );

  //Plot_Comparison_histos( inputfilenames, layer+"_deadcells", "Assumed buffer depth", "Number of dead cells" );
  //canvas->Update();
  //canvas->Print( ("output/Occupancy_Comparison_"+layer+"_deadcells_"+outputfilename+".pdf").c_str() );
  //canvas->Print( ("output/Occupancy_Comparison_"+layer+"_deadcells_"+outputfilename+".cxx").c_str() );
  //
  //if (layer.compare("All_layers") == 0 ){
  //Plot_Comparison_histos( inputfilenames, layer+"_wrt_#cells", "Number of hits per cell", "Number of cells" );
  //canvas->Update();
  //canvas->Print( ("output/Occupancy_Comparison_"+layer+"_wrt_#cells_"+outputfilename+".pdf").c_str() );
  //canvas->Print( ("output/Occupancy_Comparison_"+layer+"_wrt_#cells_"+outputfilename+".cxx").c_str() );
  //}
  //else {
  //Plot_Comparison_histos( inputfilenames, layer+"_numcells", "Number of hits per cell", "Number of cells" );
  //canvas->Update();
  //canvas->Print( ("output/Occupancy_Comparison_"+layer+"_numcells_"+outputfilename+".pdf").c_str() );
  //canvas->Print( ("output/Occupancy_Comparison_"+layer+"_numcells_"+outputfilename+".cxx").c_str() );
  //}
}

void Plot_Comparison_histos(std::vector< std::string > inputfilenames, std::string histoname, std::string x_title, std::string y_title){
  TLegend* legend;
  if(inputfilenames.size()<3) legend = new TLegend(0.3,0.6267,0.8,0.86);
  else                         legend = new TLegend(0.3,0.6267,0.8,0.86);
  legend->SetFillColor(kWhite);
  legend->SetMargin(0.1);
  
  std::vector< TH1F* > histos;
	for(size_t no_files = 0; no_files < inputfilenames.size(); ++ no_files){
    TFile* inputfile = new TFile( inputfilenames.at(no_files).c_str() );
    std::ifstream ifile( inputfilenames.at(no_files).c_str() );
    if( (bool)ifile == 1){
      TH1F* temp = new TH1F();
      temp = (TH1F*) inputfile->Get( histoname.c_str() );
      histos.push_back(temp);
    }
		else{
			std::cout<<"Error! File "<< inputfilenames.at(no_files) << " not found!";
			exit(1);
		}
  }
  double max=GetMinMaxForMultipleOverlappingHistograms(histos,true).second;
  std::string name;
  for(size_t no_histo = 0; no_histo < histos.size(); ++ no_histo){
    name = " ";
    histos.at(no_histo)->SetMinimum( std::pow(10.0,-9.0) );
    histos.at(no_histo)->SetMaximum( max );
    histos.at(no_histo)->SetLineWidth(2);
    histos.at(no_histo)->SetMarkerSize(0.7);
    if(no_histo == 0){
      name = "set (A): old L*, w/o antiDiD";
      //name = "set TDR";
      histos.at(no_histo)->SetLineColor(1);
      histos.at(no_histo)->SetMarkerColor(1);
      histos.at(no_histo)->SetMarkerStyle(4);
      histos.at(no_histo)->GetXaxis()->SetTitle( x_title.c_str() );
      histos.at(no_histo)->GetYaxis()->SetTitle( y_title.c_str() );
      //histos.at(no_histo)->Draw("hist,p");
      histos.at(no_histo)->Draw();
    }
    else{
      if(no_histo == 1){
        histos.at(no_histo)->SetLineColor(2);
        histos.at(no_histo)->SetMarkerColor(2);
        histos.at(no_histo)->SetMarkerStyle(8);
        name = "set (A): old L*, w antiDiD";
        //name = "set (A): TDR + Emittance_x";
      }
      if(no_histo == 2){
        histos.at(no_histo)->SetLineColor(3);
        histos.at(no_histo)->SetMarkerColor(3);
        histos.at(no_histo)->SetMarkerStyle(25);
        name = "set (A): new L*, w/o antiDiD";
        //name = "set (B): TDR + Emittance_x + Beta_x";
      }
      if(no_histo == 3){
        histos.at(no_histo)->SetLineColor(4);
        histos.at(no_histo)->SetMarkerColor(4);
        histos.at(no_histo)->SetMarkerStyle(21);
        name = "set (A): new L*, w antiDiD";
        //name = "set (C): TDR + Emittance_x + Beta_x + Beta_y";
      }
      //histos.at(no_histo)->Draw("hist,p,SAMES");
      histos.at(no_histo)->Draw("SAMES");
    }
    legend->AddEntry(histos.at(no_histo),name.c_str(),"p");
  }
  legend->Draw();
}
