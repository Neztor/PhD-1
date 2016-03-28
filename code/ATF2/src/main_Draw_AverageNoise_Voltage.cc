#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TF1.h"
#include "TLatex.h"

#include "Style.h"

#include "math.h"
#include <vector>
#include <array>
#include <iostream>
#include <sstream>

int const n = 9; //Number of voltages for which the noise was recorded

void GetAverageSignals(float* NoiseAverage, bool GetError,  const float voltages[], const int num_voltages, TTree* tree, const int* signal_branch, const int* voltage_branch);

int main(int const argc, char const * const * const argv) {
  UsePhDStyle();
  //The input is a TTree ROOT file(s)
  //The output is .pdf and .C files

  std::string inputfilename;
  std::string outputfilename;

  bool inputfile_set = false;
  bool outputfile_set = false;

  for (int i = 1; i < argc; i++) {
    if (argv[i] == std::string("-i")){
      if (argv[i + 1] != std::string("-o")
          && argv[i + 1] != NULL) {
        inputfilename = argv[i + 1];
        inputfile_set = true;
      } else {
        std::cerr << "You didn't give an argument for the input filename!"
          << std::endl;
      }
    }
    if (argv[i] == std::string("-o")) {
      if (argv[i + 1] != NULL 
          && argv[i + 1] != std::string("-i")) {
        outputfilename = argv[i + 1];
        outputfile_set = true;
      } else {
        std::cerr << "You didn't give an argument for the output filename!"
          << std::endl;
      }
    }
  }
  if (!inputfile_set || !outputfile_set) {
    std::cerr
      << "You didn't give the name for the inputfiles, the outputfile, or the beamintensity1. Please try again!"
      << std::endl;
    exit(1);
  }
  std::cout << "Inputfile: " << inputfilename << std::endl;
  std::cout << "Output: " << outputfilename << std::endl;

  TFile* inputfile = TFile::Open(inputfilename.c_str());
  TTree* Detector = nullptr;
  inputfile->GetObject("Tree_Detector1", Detector);

  //Set the branches
  Detector->SetBranchStatus("*", 0);
  Detector->SetBranchStatus("Signal",1);
  Detector->SetBranchStatus("Voltage",1);

  int signal = 0;
  int voltage = 0;

  Detector->SetBranchAddress("Signal", &signal);
  Detector->SetBranchAddress("Voltage", &voltage);
 
  float Voltages[n] = {0,500,700,800,900,1000,1100,1200,1300};
  float VoltagesError[n] = {2,2,2,2,2,2,2,2,2};
  
  //Fill the arrays with the average and the RMS of the signals from the TTree for the different beam intensities:
  float NoiseAverage[n];
  GetAverageSignals(NoiseAverage, false, Voltages, n, Detector, &signal, &voltage);
  float NoiseAverageError[n];
  GetAverageSignals(NoiseAverageError, true, Voltages, n, Detector, &signal, &voltage);
  
  TGraphErrors* AverageNoise_Voltage = new TGraphErrors(n,Voltages,NoiseAverage,VoltagesError,NoiseAverageError);
  AverageNoise_Voltage->SetTitle("Average noise signal for different PMT voltages;Voltage [V];Average RHUL cherenkov noise [a.u.]");
  AverageNoise_Voltage->SetMarkerSize(0.85);
  AverageNoise_Voltage->SetMarkerColorAlpha(4, 0.5);
  AverageNoise_Voltage->SetMarkerStyle(8);

  //Plot the TGraphErrors for the different intensities onto the same canvas:
  TCanvas* canvas = new TCanvas();
  //TF1 *linearfit = new TF1("fit","pol1",750,1350);
  //linearfit->SetParName(0,"NoiseOffset");
  //linearfit->SetParName(1,"Slope");
  //linearfit->SetLineColor(2);
  //gStyle->SetOptFit(0);
  AverageNoise_Voltage->Draw("APE");
  //AverageNoise_Voltage->Fit("fit","RMEX0");
  //double Slopeval = linearfit->GetParameter(1);
  //double SlopeErrorval = linearfit->GetParError(1);
  //std::stringstream Slopeval_string (std::stringstream::in | std::stringstream::out);
  //Slopeval_string << "Slope: " << Slopeval << " #pm " << SlopeErrorval;
  //TLatex *t = new TLatex();
  //t->SetNDC();
  //t->SetTextAlign(13);
  //t->SetTextFont(63);
  //t->SetTextSizePixels(22);
  //t->DrawLatex(.3,.8,Slopeval_string.str().c_str());

  std::string outputname1 = "output/" + outputfilename + ".pdf";
  std::string outputname2 = "output/" + outputfilename + ".cxx";
  canvas->Print(outputname1.c_str());
  canvas->Print(outputname2.c_str());

  inputfile->Close();
}  

void GetAverageSignals(float* NoiseAverage, bool GetError,  const float voltages[], const int num_voltages, TTree* tree, const int* signal_branch, const int* voltage_branch){
  std::string title1D = "RHUL cherenkov detector signal without beam;Noise [a.u.];Count";
  std::vector<TH1D*> Noise_Voltage;
  //Push back as many TH1 histograms as needed in order to have one for each voltage
  for(int number_voltages = 0; number_voltages < num_voltages; ++number_voltages){
    Noise_Voltage.emplace_back(new TH1D("noise", title1D.c_str(),1000,0,10000));
  }

  long long int const entries =  tree->GetEntries();
  for (long long int i = 0; i < entries; ++i){
    tree->GetEntry(i);
    if(*signal_branch > 0){
      for(int number_voltages = 0; number_voltages < num_voltages; ++number_voltages){
        //Fill the TH1 in the vector with noise signals for a voltage, that corresponds to the desired voltages in the voltage vector:
        if(*voltage_branch > voltages[number_voltages]-1 && *voltage_branch < voltages[number_voltages]+1){
          Noise_Voltage.at(number_voltages)->Fill(*signal_branch);
        }
      }
    }
  }

  for(size_t iterator; iterator < Noise_Voltage.size();++iterator){
    //If the average signal is desired, get the mean from the signal distributions in the TH1 vector
    if (GetError==false){
      NoiseAverage[iterator] = Noise_Voltage.at(iterator)->GetMean();  
    }
    //If the error is desired, get the RMS from the signal distributions in the TH1 vector, and divide it by the square root of number of entries
    //Then you have the standard deviation of the mean
    else{
      NoiseAverage[iterator] = Noise_Voltage.at(iterator)->GetRMS()/std::sqrt(Noise_Voltage.at(iterator)->GetEntries());  
    }
    delete Noise_Voltage.at(iterator);
  }
}