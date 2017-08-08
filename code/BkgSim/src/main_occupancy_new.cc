#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TPaveStats.h"


#include <bitset>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <limits>
#include <string>
#include <vector>

#include "UsefulFunctions.h"
#include "GeneralFunctions_SiDBkgSim_new.h"
#include "CellHits_class_new.h"
#include "Subdetector_class_new.h"

using namespace std;

double CalculatePhi(double x, double y);
int CalculateLayer(long long int const id, Subdetector const & SubDetector); 
long long int MakeNewCellID(double const x, double const y, Subdetector const & component);
void Draw_single_plots ( TH1D* histo, TCanvas* canvas, bool normalize, int integral_startbin, bool integral_numhits);
void Draw_multiple_plots (std::vector< TH1D* > histos, TCanvas* canvas, bool normalize, int integral_startbin, bool integral_numhits);
void Print_multiple_plots_from_same_vec (std::vector< TH1D* > histos, TCanvas* canvas, bool normalize, int integral_startbin, bool integral_numhits, std::string output);

double weight = 1.0;

int main(int const argc, char const * const * const argv) {
  std::vector<std::string> *inputfilenames = new std::vector<std::string>();
  std::string argument_subdetectors;

  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
  std::string outputfile_name = oss.str();

  int NUMBER_OF_FILES = 0;
  bool NUMBER_OF_FILES_set = false;
  bool inputfile_set = false;
  bool subdetector_set = false;

  for (int i = 1; i < argc; i++) {
    if (argv[i] == std::string("-n")) {
      if (argv[i + 1] != NULL && 
          argv[i + 1] != std::string("-s") && 
          argv[i + 1] != std::string("-i")) {
        NUMBER_OF_FILES = std::stoi(argv[i + 1]);
        std::cout << "Number of input files = " << NUMBER_OF_FILES << std::endl;
        NUMBER_OF_FILES_set = true;
      } else {
        std::cerr << "You didn't give an argument for the number of files!" << std::endl;
      }
    }
  }
  for (int i = 1; i < argc; i++) {
    if (argv[i] == std::string("-i") && argv[i + 1] != std::string("-n") && argv[i + 1] != std::string("-s")) {
      if (argv[i + 1] != NULL) {
        int j = 1;
        do {
          if (argv[i + j] != std::string("-n") && argv[i + j] != std::string("-s")) {
            inputfilenames->push_back(argv[i + j]);
            ++j;
          } else {
            break;
          }
        } while (j <= NUMBER_OF_FILES);
        inputfile_set = true;
      } else {
        std::cerr << "You didn't give an argument for the inputfile(s)!" << std::endl;
      }
    }
    if (argv[i] == std::string("-s")) {
      if (argv[i + 1] != NULL && 
          argv[i + 1] != std::string("-n") && 
          argv[i + 1] != std::string("-o")) {
        argument_subdetectors = argv[i + 1];
        subdetector_set = true;
      } else {
        std::cerr << "You didn't give an argument for the subdetector!" << std::endl;
      }
    }
    if (argv[i] == std::string("-o")) {
      if (argv[i + 1] != NULL && 
          argv[i + 1] != std::string("-n") && 
          argv[i + 1] != std::string("-s")) {
        outputfile_name = argv[i + 1];
      } else {
        std::cerr << "You didn't give an argument for the outputfile name!" << std::endl;
      }
    }
  }
  if (!inputfile_set || !subdetector_set || !NUMBER_OF_FILES_set) {
    std::cerr
      << "You didn't give the name for the subdector, the inputfiles or the amount of files. Please try again!"
      << std::endl;
    exit(1);
  }


  Subdetector det( argument_subdetectors );

  bool endcap = false;
  bool barrel = false;
  if (det.getShape().find("endcap") != std::string::npos){//If Endcap, calculate CellID with x and y
    std::cout <<"Found 'endcap'!" << std::endl;
    endcap = true;
  }
  else if (det.getShape().find("barrel") != std::string::npos){
    std::cout <<"Found 'barrel'!" << std::endl;
    barrel = true;
  }
  else{
    std::cerr << "The given subdetector shape was not recognized!" << std::endl; 
    exit(-1);
  }
  bool Silicon = false;
  bool Calo = false;
  if (det.getName().find("Si") != std::string::npos){
    std::cout << "Silicon detector found!" << std::endl;
    Silicon = true;
  }
  else if (det.getName().find("cal",0) != std::string::npos
      || det.getName().find("Cal",0) != std::string::npos
      || det.getName().find("Muon",0) != std::string::npos){
    std::cout << "Calorimeter found!" << std::endl;
    Calo = true;
  }
  else{
    std::cerr << "The given subdetector name was not recognized!" << std::endl; 
    exit(-1);
  }

  CellHits cellhits( &det );
  for (int file_iterator = 0; file_iterator < NUMBER_OF_FILES; ++file_iterator) {
    TFile *file = TFile::Open(inputfilenames->at(file_iterator).c_str());
    TTree *tree = Get_TTree(file, det.getName());

    //Set the branches
    int pdg(0);
    int HitCellID0(0);
    double D_HitPosition_x(0.0), D_HitPosition_y(0.0), D_HitPosition_z(0.0); //double for Silicon detectors
    float F_HitPosition_x(0.0), F_HitPosition_y(0.0), F_HitPosition_z(0.0);
    double HitPosition_x;
    double HitPosition_y;
    double HitPosition_z;

    tree->SetBranchStatus("*", 0);
    tree->SetBranchStatus("HitCellID0", 1);
    tree->SetBranchAddress("HitCellID0", &HitCellID0);

    tree->SetBranchStatus("HitPosition_x", 1);
    tree->SetBranchStatus("HitPosition_y", 1);
    tree->SetBranchStatus("HitPosition_z", 1);

    if (Silicon){
      tree->SetBranchStatus("HitParticle_PDG", 1);
      tree->SetBranchAddress("HitParticle_PDG", &pdg);

      tree->SetBranchAddress("HitPosition_x", &D_HitPosition_x);
      tree->SetBranchAddress("HitPosition_y", &D_HitPosition_y);
      tree->SetBranchAddress("HitPosition_z", &D_HitPosition_z);
    }
    else if (Calo){
      tree->SetBranchStatus("HitMotherParticle_PDG", 1);
      tree->SetBranchAddress("HitMotherParticle_PDG", &pdg);

      tree->SetBranchAddress("HitPosition_x", &F_HitPosition_x);
      tree->SetBranchAddress("HitPosition_y", &F_HitPosition_y);
      tree->SetBranchAddress("HitPosition_z", &F_HitPosition_z);
    }
    else{
      std::cerr << "This subdetector name was not recognized!" << std::endl; 
      exit(-1);
    }


    //Now we loop through the tree
    //Combine the two Cell ID's into a single new Cell ID
    //See how often the new Cell ID occurs in total, this is the occupancy

    long long int const entries = tree->GetEntries();
    for (long long int i = 0; i < entries; ++i) {
      tree->GetEntry(i);
      if (Silicon){
        HitPosition_x = D_HitPosition_x;
        HitPosition_y = D_HitPosition_y;
        HitPosition_z = D_HitPosition_z;
      }
      else if (Calo){
        HitPosition_x = F_HitPosition_x;
        HitPosition_y = F_HitPosition_y;
        HitPosition_z = F_HitPosition_z;
      }
      //if (pdg != 13 && pdg != -13) continue;
      //if (endcap && *HitPosition_z < 0) continue;
      std::cout << "HitPosition_x = " << HitPosition_x << std::endl;
      std::cout << "HitPosition_y = " << HitPosition_y << std::endl;
      std::cout << "HitPosition_z = " << HitPosition_z << std::endl;
      //Make a combined cell ID
      long long int HitCellID1 = 0;
      if (endcap){
        HitCellID1 = MakeNewCellID(HitPosition_x,HitPosition_y,det);
      }
      else if (barrel){
        double phi = CalculatePhi(HitPosition_x, HitPosition_y);
        //If Barrel, calculate CellID with z and phi on a certain radius
        if (Silicon) HitCellID1 = MakeNewCellID(HitPosition_z,phi*det.getRMin().at( CalculateLayer(HitCellID0,det)-1 ),det);//-1 for Silicon detectors only, because layer count starts from 1
        else if (Calo) HitCellID1 = MakeNewCellID(HitPosition_z,phi*det.getRMin().at( CalculateLayer(HitCellID0,det) ),det);
      }
      else{
        std::cerr << "This subdetector shape was not recognized!" << std::endl; 
        exit(-1);
      }
      std::cout << "HitCellID1 = " << HitCellID1 << std::endl;
      long long int const combined_cell_id = (long long) HitCellID1 << 32 | HitCellID0;
      //Use the CellHits class for storing the hit cells and their hitcounts
      cellhits.Check_CellID(combined_cell_id, HitPosition_x, HitPosition_y, HitPosition_z);
    }
    file->Close();
  }

  std::string subdetectorname = det.getName();
	TFile* Outputfile = new TFile(("output/Occupancy_"+subdetectorname+"_"+outputfile_name+".root").c_str(),"RECREATE");
  //Make histogram vectors for storing the histograms
  std::vector < std::string > all_name;
  all_name.emplace_back( "All_layers" );
  all_name.emplace_back( "All_layers_wrt_#cells" );
  all_name.emplace_back( "All_layers_losthits" );
  all_name.emplace_back( "All_layers_deadcells" );
  std::vector < std::string > title;
  title.emplace_back( "Occupancy for " + subdetectorname + ";Number of hits per cell;Number of cells" );
  title.emplace_back( "Occupancy for " + subdetectorname + " wrt to tot # cells;Number of hits per cell;Number of cells" );
  title.emplace_back( "Number of hits lost for a given buffer depth for " + subdetectorname + ";Assumend buffer depth;Number of hits lost" );
  title.emplace_back( "Number of dead cells for a given buffer depth for " + subdetectorname + ";Assumend buffer depth;Number of dead cells" );

  std::vector< TH1D* > histos;
  std::vector< TH1D* > histos_numcells;
  std::vector< TH1D* > histos_bufferdepth;
  std::vector< TH1D* > histos_deadcells;

  //Find out the maximum number of hits per cell and the total number of hits overall
  int tot_no_hits;
  int max_no_hits = 0;
  for (size_t vecpos = 0; vecpos < cellhits.Get_HitCount().size(); ++vecpos) {
    if (cellhits.Get_HitCount().at(vecpos) > max_no_hits){
      max_no_hits = cellhits.Get_HitCount().at(vecpos);
    }
    tot_no_hits += cellhits.Get_HitCount().at(vecpos);
  }
  int xrange = max_no_hits + max_no_hits/10;
  int max_num_layers = det.getNumberOfLayers();

  //Make the histograms
  TH1D* All_Layers_histo             = new TH1D(all_name.at(0).c_str(), title.at(0).c_str(), xrange, 0, xrange);
  TH1D* All_Layers_histo_numcells    = new TH1D(all_name.at(1).c_str(), title.at(1).c_str(), xrange, 0, xrange);
  TH1D* All_Layers_histo_bufferdepth = new TH1D(all_name.at(2).c_str(), title.at(2).c_str(), xrange, 0, xrange);
  TH1D* All_Layers_histo_deadcells   = new TH1D(all_name.at(3).c_str(), title.at(3).c_str(), xrange, 0, xrange);

  for (int number_layer = 0; number_layer < max_num_layers; ++number_layer) {
    std::stringstream layername, layername2, layername3, layername4;
    layername << "Layer_" << number_layer;
    layername2 << "Layer_" << number_layer << "_numcells";
    layername3 << "Layer_" << number_layer << "_losthits";
    layername4 << "Layer_" << number_layer << "_deadcells";

    TH1D* temp1 = new TH1D(layername.str().c_str(), title.at(0).c_str(), xrange, 0, xrange);
    TH1D* temp2 = new TH1D(layername2.str().c_str(), title.at(1).c_str(), xrange, 0, xrange);
    TH1D* temp3 = new TH1D(layername3.str().c_str(), title.at(2).c_str(), xrange, 0, xrange);
    TH1D* temp4 = new TH1D(layername4.str().c_str(), title.at(3).c_str(), xrange, 0, xrange);
    histos.push_back(            temp1);
    histos_numcells.push_back(   temp2);
    histos_bufferdepth.push_back(temp3);
    histos_deadcells.push_back(  temp4);
  }
  //Filling the primary histograms with the entries from the cellhits
  for (size_t vecpos = 0; vecpos < cellhits.Get_HitCount().size(); ++vecpos) {
    if(cellhits.Get_HitCount().at(vecpos) > 0){
      //std::cout << "Layer: " << cellhits.Get_Layer().at(vecpos) << std::endl;
      int current_layer = cellhits.Get_Layer().at(vecpos);
      if (Silicon) histos.at(current_layer -1 )->Fill( cellhits.Get_HitCount().at(vecpos),weight );//-1 for Silicon detectors only, because layer count starts from 1
      else if (Calo) histos.at(current_layer)->  Fill( cellhits.Get_HitCount().at(vecpos),weight );
      All_Layers_histo->Fill( cellhits.Get_HitCount().at(vecpos),weight );
    }
  }
  //Filling numcells plots:
  long long int tot_num_cells = 0;
  long long int tot_num_hitcells = 0;
  for (int number_layer = 0; number_layer < max_num_layers; ++number_layer) {
    tot_num_cells += det.getNumberOfCells().at(number_layer);
    for (int bin = 2; bin < histos.at(number_layer)->GetNbinsX(); ++bin) {
      tot_num_hitcells += histos.at(number_layer)->GetBinContent(bin);
      histos_numcells.at(number_layer)->SetBinContent(bin, histos.at(number_layer)->GetBinContent(bin) );
    }
    histos.at( number_layer )->SetBinContent(1, det.getNumberOfCells().at(number_layer) - tot_num_hitcells );
    histos_numcells.at( number_layer )->SetBinContent(1, det.getNumberOfCells().at(number_layer));
    tot_num_hitcells = 0;
  }
  for (int bin = 2; bin < All_Layers_histo->GetNbinsX(); ++bin) {
    tot_num_hitcells += All_Layers_histo->GetBinContent(bin);
    All_Layers_histo_numcells->SetBinContent(bin, All_Layers_histo->GetBinContent(bin));
  }
  All_Layers_histo->SetBinContent(1, tot_num_cells - tot_num_hitcells);
  All_Layers_histo_numcells->SetBinContent(1, tot_num_cells);

  //Filling bufferdepth plots:
  for (int number_layer = 0; number_layer < max_num_layers; ++number_layer) {
    for (int i = 0; i <= max_no_hits; ++i){//For each bufferdepth
      long long int tot = 0;
      long long int deadcells = 0;
      for (int bin = i+1; bin < histos.at(number_layer)->GetNbinsX(); ++bin) {//go through the histo from bufferdepth value onwards
        tot += histos.at(number_layer)->GetBinContent(bin) * (histos.at(number_layer)->GetBinLowEdge(bin) - i);//Sum the total number of hits in each of these bins
        deadcells += histos.at(number_layer)->GetBinContent(bin);
      }
      histos_bufferdepth.at(number_layer)->SetBinContent(i+1, tot);
      histos_deadcells.at(number_layer)->SetBinContent(i+1, deadcells);
    }
  }
  for (int i = 0; i <= max_no_hits; ++i){//For each bufferdepth
    long long int tot = 0;
    long long int deadcells = 0;
    for (int bin = i+1; bin < All_Layers_histo->GetNbinsX(); ++bin) {//go through the histo from bufferdepth value onwards
      tot += All_Layers_histo->GetBinContent(bin) * (All_Layers_histo->GetBinLowEdge(bin) - i);//Sum the total number of hits in each of these bins
      deadcells += All_Layers_histo->GetBinContent(bin);
    }
    All_Layers_histo_bufferdepth->SetBinContent(i+1, tot);
    All_Layers_histo_deadcells->SetBinContent(i+1, deadcells);
  }

  std::cout<< "---------------" <<std::endl;
  std::cout<< "Total number of hits counted for subdetector "<< subdetectorname <<std::endl;
  std::cout << tot_no_hits <<std::endl;
  std::cout<< "---------------" <<std::endl;

  //Plot the histogram and save it
  TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
  canvas->SetLogy(1);

  std::stringstream output;
  output << "output/occupancy_" << subdetectorname << "_" << outputfile_name;
  Print_multiple_plots_from_same_vec (histos, canvas, false, 2, true,  output.str());

  std::stringstream output2;
  output2 << "output/occupancy_numcells_" << subdetectorname << "_" << outputfile_name;
  Print_multiple_plots_from_same_vec (histos_numcells, canvas, true, 2, true,  output2.str());

  std::stringstream output3;
  output3 << "output/occupancy_bufferdepth_" << subdetectorname << "_" << outputfile_name;
  Print_multiple_plots_from_same_vec (histos_bufferdepth, canvas, true, 1, false,  output3.str());

  std::stringstream output4;
  output4 << "output/occupancy_deadcells_" << subdetectorname << "_" << outputfile_name;
  Print_multiple_plots_from_same_vec (histos_deadcells, canvas, true, 2, false, output4.str());


  Draw_single_plots( All_Layers_histo,canvas, false, 2, true); 
  std::stringstream All_output;
  All_output << "output/occupancy_all_layers_" << subdetectorname << "_" << outputfile_name;
  canvas->Print((All_output.str() + ".pdf").c_str());
  canvas->Print((All_output.str() + ".cxx").c_str());

  Draw_single_plots ( All_Layers_histo_numcells,canvas, true, 2, true); 
  std::stringstream All_numcells_output1;
  All_numcells_output1 << "output/occupancy_numcells_all_layers_" << subdetectorname << "_" << outputfile_name;
  canvas->Print((All_numcells_output1.str() + ".pdf").c_str());
  canvas->Print((All_numcells_output1.str() + ".cxx").c_str());

  Draw_single_plots ( All_Layers_histo_bufferdepth,canvas, true, 1, false);
  std::stringstream All_bufferdepth_output1;
  All_bufferdepth_output1 << "output/occupancy_bufferdepth_all_layers_" << subdetectorname << "_" << outputfile_name;
  canvas->Print((All_bufferdepth_output1.str() + ".pdf").c_str());
  canvas->Print((All_bufferdepth_output1.str() + ".cxx").c_str());

  Draw_single_plots ( All_Layers_histo_deadcells,canvas, true, 2, false);
  std::stringstream All_deadcells_output1;
  All_deadcells_output1 << "output/occupancy_deadcells_all_layers_" << subdetectorname << "_" << outputfile_name;
  canvas->Print((All_deadcells_output1.str() + ".pdf").c_str());
  canvas->Print((All_deadcells_output1.str() + ".cxx").c_str());
  
	Outputfile->Write();
  return 0;
}

int CalculateLayer(long long int const id, Subdetector const & SubDetector) {
  std::bitset<64> cellidbit(id);
  std::string CellID_ = cellidbit.to_string();
  int LayerInt = -1;
  std::stringstream LayerID;

  //This for loop calculates the layer id
  //From a sring of 0's and 1's, e.g. 00001011010010
  //The StartBin is the first bin in the string we are interested in (when reading from right to left)
  //The LengthBin is the length of the string we are interested in
  //We read from left to right, but we specify the start position from right to left
  //There is a magic +1 in there because strings start at element 0.
  for (int i = CellID_.size() - (SubDetector.getStartBitLayer() + SubDetector.getLengthBitLayer()); i <= CellID_.size() - (SubDetector.getStartBitLayer() + 1);
      ++i) {
    LayerID << CellID_.at(i);
  }

  std::bitset<64> LayerIDbit(LayerID.str());
  LayerInt = LayerIDbit.to_ulong();

  return LayerInt;
}

double CalculatePhi(double x, double y){
  double phi =0;
  if(x>0) return phi = atan(y/x);
  if(x<0 && y>=0) return phi = atan(y/x) + M_PI;
  if(x<0 && y<0)  return phi = atan(y/x) - M_PI;
  if(x==0 && y>0)  return phi = 0.5*M_PI;
  if(x==0 && y<0)  return phi = -0.5*M_PI;
  else return -101;
}
long long int MakeNewCellID(double const x, double const y, Subdetector const & component){
  int newX = static_cast<int>(x/component.getCellSizeX()); //Check if Cell Size Area is the same as Cell Dimension
  int newY = static_cast<int>(y/component.getCellSizeY()); //Check if Cell Size Area is the same as Cell Dimension
  if(x >= 0) ++newX;
  if(y >= 0) ++newY;
  bitset<32> bitY = newY;
  newY = 0;
  for(int i = 0; i < 31; ++i){
    newY += bitY[i]*pow(2,i);
  }
  return (long long int) newX << 32 | newY;
}

void Draw_single_plots ( TH1D* histo, TCanvas* canvas, bool normalize, int integral_startbin, bool integral_numhits){
  int tot = 0;
  for(int bin = integral_startbin; bin <= histo->GetNbinsX(); ++bin){
    if (integral_numhits == true) tot += histo->GetBinContent(bin)*histo->GetBinLowEdge(bin);
    else tot += histo->GetBinContent(bin);
  }
  histo->SetStats(0);
  histo->Sumw2(1);
  //histo->Scale(weight);
  histo->SetMinimum(0.1);
  if(normalize == true){
    histo->Scale(1.0/histo->GetBinContent(1));
    histo->SetMinimum( pow(10,-12) );
  }
  histo->SetLineColor(2);
  histo->Draw("hist");
  canvas->Update();

  TPaveText *text1 = new TPaveText(0.75,0.8,0.95,0.9,"brNDC");
  text1->SetTextFont(62);
  text1->SetTextColor(2);
  text1->SetFillColor(0);
  std::stringstream title;
  title << histo->GetName();
  text1->AddText(title.str().c_str());
  text1->AddLine(0,0.5,1,0.5);
  std::stringstream entries;
  entries << "Entries = " << tot; 
  text1->AddText(entries.str().c_str());
  text1->Draw();
}

void Print_multiple_plots_from_same_vec (std::vector< TH1D* > histos, TCanvas* canvas, bool normalize, int integral_startbin, bool integral_numhits, std::string output){
  Draw_multiple_plots(histos, canvas, normalize, integral_startbin, integral_numhits);
  canvas->Print((output + ".pdf").c_str());
  canvas->Print((output + ".cxx").c_str());

}

void Draw_multiple_plots (std::vector< TH1D* > histos, TCanvas* canvas, bool normalize, int integral_startbin, bool integral_numhits){
  int i = 0;
  int color = 2; // Very first histogram will be drawn with the color 2, then counted up
  int marker = 20; // Very first histogram will be drawn with the marker style 20, then counted up
  for (size_t number_histo = 0; number_histo< histos.size(); ++number_histo) {
    histos.at(number_histo)->SetStats(0);
    histos.at(number_histo)->Sumw2(1);
    //histos.at(number_histo)->Scale(weight);
  }
  double max=GetMinMaxForMultipleOverlappingHistograms(histos,true).second;
  for (size_t number_histo = 0; number_histo< histos.size(); ++number_histo) {
    if(number_histo == 0){
      int tot = 0;
      for(int bin = integral_startbin; bin <= histos.at(number_histo)->GetNbinsX(); ++bin){
        if (integral_numhits == true) tot += histos.at(number_histo)->GetBinContent(bin)*histos.at(number_histo)->GetBinLowEdge(bin);
        else tot += histos.at(number_histo)->GetBinContent(bin);
      }
      if(normalize == true){
        histos.at(number_histo)->Scale(1.0/histos.at(number_histo)->GetBinContent(1));
        histos.at(number_histo)->SetMinimum( pow(10,-12) );
      }
      else{
        histos.at(number_histo)->SetMaximum(max);
        histos.at(number_histo)->SetMinimum(0.1);
      }
      histos.at(number_histo)->SetLineColor(color);
      histos.at(number_histo)->SetMarkerColor(color);
      histos.at(number_histo)->SetMarkerStyle(marker);
      histos.at(number_histo)->Draw("P");
      canvas->Update();
      TPaveText *text1;
      if (histos.size() > 5){
        text1 = new TPaveText(0.6,0.8,0.8,0.9,"brNDC");
      }
      else{
        text1 = new TPaveText(0.75,0.8,0.95,0.9,"brNDC");
      }
      text1->SetTextFont(62);
      text1->SetTextColor(color);
      text1->SetFillColor(0);
      std::stringstream title;
      title << histos.at(number_histo)->GetName();
      text1->AddText(title.str().c_str());
      text1->AddLine(0,0.5,1,0.5);
      std::stringstream entries1;
      entries1 << "Entries = " << tot; 
      text1->AddText(entries1.str().c_str());
      text1->Draw();
      i=1;
    }
    if(number_histo > 0){
      int tot = 0;
      for(int bin = integral_startbin; bin <= histos.at(number_histo)->GetNbinsX(); ++bin){
        if (integral_numhits == true) tot += histos.at(number_histo)->GetBinContent(bin)*histos.at(number_histo)->GetBinLowEdge(bin);
        else tot += histos.at(number_histo)->GetBinContent(bin);
      }
      if(normalize == true){
        histos.at(number_histo)->Scale(1.0/histos.at(number_histo)->GetBinContent(1));
        histos.at(number_histo)->SetMinimum( pow(10,-12) );
      }
      else{
        histos.at(number_histo)->SetMaximum(max);
        histos.at(number_histo)->SetMinimum(0.1);
      }
      color++;
      marker++;
      if(color == 5 || color == 10) color += 1; // 5 would be yellow, 10 would be very light gray 
      histos.at(number_histo)->SetLineColor(color);
      histos.at(number_histo)->SetMarkerColor(color);
      histos.at(number_histo)->SetMarkerStyle(marker);
      histos.at(number_histo)->Draw("P,SAMES");
      canvas->Update();
      TPaveText *text2;
      if (histos.size() > 5) {
        if(number_histo >= 5){
          if(number_histo == 5) {
            i=0;
          }
          text2 = new TPaveText(0.75,0.8-i*0.1,0.95,0.9-i*0.1,"brNDC");
        }
        else {
          text2 = new TPaveText(0.6,0.8-i*0.1,0.8,0.9-i*0.1,"brNDC");
        }
      } 
      else{
        text2 = new TPaveText(0.75,0.8-i*0.1,0.95,0.9-i*0.1,"brNDC");
      }
      text2->SetTextFont(62);
      text2->SetTextColor(color);
      text2->SetFillColor(0);
      //text->AddLine(0,0.5,1,0.5);
      std::stringstream title2;
      title2 << histos.at(number_histo)->GetName();
      text2->AddText(title2.str().c_str());
      text2->AddLine(0,0.5,1,0.5);
      std::stringstream entries2;
      entries2 << "Entries = " << tot;//-1 because of entries in first bin count 1 because of setbincontent
      text2->AddText(entries2.str().c_str());
      text2->Draw();
      i++;
    }
  }
}
