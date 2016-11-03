#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TPaveStats.h"
#include "TLine.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

#include "Helix_class.h"
#include "GeneralFunctions_SiDBkgSim.h"

#include "Style.h"

using namespace std;

int main(int const argc, char const * const * const argv) {
				UsePhDStyle();

				std::vector<std::string> *inputfilenames = new std::vector<std::string>();

				int NUMBER_OF_FILES = 0;
				bool NUMBER_OF_FILES_set = false;
				bool inputfile_set = false;

				for (int i = 1; i < argc; i++) {
								if (argv[i] == std::string("-n")) {
												if (argv[i + 1] != NULL 
																				&& argv[i + 1] != std::string("-s") 
																				&& argv[i + 1] != std::string("-n") 
																				&& argv[i + 1] != std::string("-t") 
																				&& argv[i + 1] != std::string("-i")) {
																NUMBER_OF_FILES = std::stoi(argv[i + 1]);
																std::cout << "Number of input files = " << NUMBER_OF_FILES << std::endl;
																NUMBER_OF_FILES_set = true;
												} else {
																std::cerr << "You didn't give an argument for the number of files!" << std::endl;
												}
								}
				}
				for (int i = 1; i < argc; i++) {
								if (argv[i] == std::string("-i")){
												if (argv[i + 1] != NULL) {
																int j = 1;
																do {
																				if (argv[i + j] != std::string("-n") 
																												&& argv[i + j] != std::string("-i") 
																												&& argv[i + j] != std::string("-n") 
																												&& argv[i + j] != std::string("-s") 
																												&& argv[i + j] != std::string("-t")) {
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
				}
				if (!inputfile_set || !NUMBER_OF_FILES_set) {
								std::cerr
												<< "You didn't give the name for the inputfiles or the amount of files. Please try again!"
												<< std::endl;
								exit(1);
				}

				//Make histogram for storing the information
				float const zmin = 0.0;
				float const zmax = 300.0;
				int const zbin = 10000;
        float const ymin = -29;
        float const ymax = 29;
				int const ybin = 100;
        float const xmin = -29;
        float const xmax = 29;
				int const xbin = 100;

				std::string const title_x = "Pairs spiraling in the magnetic field;z [mm];x [mm];# of particles per (0.03mm x 0.58mm)";
				std::string const title_y = "Pairs spiraling in the magnetic field;z [mm];y [mm];# of particles per (0.03mm x 0.58mm)";
				TH2D* histo_x = new TH2D("Helix_tracks_xz", title_x.c_str(), zbin,zmin,zmax, xbin, xmin, xmax);
				TH2D* histo_y = new TH2D("Helix_tracks_yz", title_y.c_str(), zbin,zmin,zmax, ybin, ymin, ymax);

				//TTree for outputfile -> store new x, y and z positions of the helixes in there 
				std::string specialname = "1bunch_updated_BeamPipe";
				TFile* Outputfile = new TFile(("output/Helix_in_beampipe_"+specialname+".root").c_str(),"RECREATE");
        TTree *outputtree = new TTree("Helix_Tracks","Helix_Tracks");
        double tree_x(0),tree_y(0),tree_z(0);
        outputtree->Branch("x",&tree_x);
        outputtree->Branch("y",&tree_y);
        outputtree->Branch("z",&tree_z);

				//For counting all particles drawn, and the ones leaving the beam pipe:
				long long int particles_outside_beampipe = 0;
				long long int particles_inside_beampipe = 0;
				
				//Initializing the Helix class:
				float const BField = 5.0;
				Helix helix(BField);

				//Looping through the root file(s):
				for (int file_iterator = 0; file_iterator < NUMBER_OF_FILES; ++file_iterator) {
								TFile *file = TFile::Open(inputfilenames->at(file_iterator).c_str());
								TTree *tree = Get_TTree(file, "MCP");

								//Set the branches
								double vertex_x = 0.0;
								double vertex_y = 0.0;
								double vertex_z = 0.0;
								double momentum_x = 0.0;
								double momentum_y = 0.0;
								double momentum_z = 0.0;
								float charge = -99.0;
								//bool CreatedInSimulation_Status = false;
								//float creationtime = 0.0;

								tree->SetBranchStatus("*", 0);
								tree->SetBranchStatus("Vertexx", 1);
								tree->SetBranchStatus("Vertexy", 1);
								tree->SetBranchStatus("Vertexz", 1);
								tree->SetBranchAddress("Vertexx", &vertex_x);
								tree->SetBranchAddress("Vertexy", &vertex_y);
								tree->SetBranchAddress("Vertexz", &vertex_z);
								tree->SetBranchStatus("Momentumx", 1);
								tree->SetBranchStatus("Momentumy", 1);
								tree->SetBranchStatus("Momentumz", 1);
								tree->SetBranchAddress("Momentumx", &momentum_x);
								tree->SetBranchAddress("Momentumy", &momentum_y);
								tree->SetBranchAddress("Momentumz", &momentum_z);
								tree->SetBranchStatus("Charge", 1);
								tree->SetBranchAddress("Charge", &charge);
								//tree->SetBranchStatus("CreatedInSimulation_Status", 1);
								//tree->SetBranchAddress("CreatedInSimulation_Status", &CreatedInSimulation_Status);
								//tree->SetBranchStatus("CreationTime", 1);
								//tree->SetBranchAddress("CreationTime", &creationtime);

								std::vector< double > vertex;
								std::vector< double > momentum;
								double z = zmin;
								double* helix_positions = nullptr;
								//double new_x = 0;
								//double new_y = 0;

								float const beampipe_angle1 = 3.266*M_PI/180;
								float const beampipe_angle2 = 5.329*M_PI/180;
								double const tan_beampipe_angle1( tan(beampipe_angle1) );
								double const tan_beampipe_angle2( tan(beampipe_angle2) );

								bool particle_went_outside_beampipe = false;

								//For trying out FillN:
								double* x_array = new double[zbin];
								double* y_array = new double[zbin];
								double* z_array = new double[zbin];

								long long int const entries = tree->GetEntries();
								for (long long int i = 0; i < entries; ++i) {
												tree->GetEntry(i);
												if (momentum_z < 0) continue;
												//if (CreatedInSimulation_Status == 1) continue;
												vertex = { vertex_x, vertex_y, vertex_z };
												//if (abs(vertex_x) > 0.1 || abs(vertex_y) > 0.1 ) continue;
												momentum = { momentum_x, momentum_y, momentum_z };
												//if (creationtime < 1 && sqrt(momentum_x*momentum_x+momentum_y*momentum_y) > 0.2) continue;

											  helix.Set_particlevalues(momentum, charge, vertex); // setting the constant values for the current particle in the helix class
												
												particle_went_outside_beampipe = false; //assume first that every particle will stay inside the beampipe

												//Looping through the histogramm bins in z:
												for (int step = 1; step <= zbin; ++step){
																helix_positions = helix.Get_position(z);
																x_array[step-1]=helix_positions[0]*1000.0;
																y_array[step-1]=helix_positions[1]*1000.0;
																z_array[step-1]=z;
																//new_x = helix_positions[0]*1000.0; // to convert from m to mm
																//new_y = helix_positions[1]*1000.0; // to convert from m to mm
																//histo_x->Fill(z, new_x);
																//histo_y->Fill(z, new_y);
                                //Fill the output TTree:
																tree_x = x_array[step-1];
                                tree_y = y_array[step-1];
																//tree_x = new_x;
                                //tree_y = new_y;
                                tree_z = z;
                                outputtree->Fill();
																//Check if particle leaves the beam pipe, and if yes set boolian to true -> after that the if loop should not be accessed again
																if ( particle_went_outside_beampipe == false &&
																		 ((std::abs(tree_x) > 12 && z <= 62.5) || //beam pipe inside vertex barrel: cylinder with 12mm radius, 32.5mm long
																		 (std::abs(tree_y) > 12 && z <= 62.5) ||
																		 (z > 62.5 && z <= 205 && std::abs(tree_x) > tan_beampipe_angle1*(z-62.5)+12) ||  //beam pipe outside vertex barrel: cone with half angle of beampipe_angle1, length 205-62.5mm, starting at z=62.5mm
																		 (z > 62.5 && z <= 205 && std::abs(tree_y) > tan_beampipe_angle1*(z-62.5)+12) ||
																		 (z > 205 && z <= zmax && std::abs(tree_x) > tan_beampipe_angle2*(z-205)+20.13)|| //beam pipe outside vertex barrel: cone with half angle of beampipe_angle2, length 205-62.5mm, starting at z=205mm
																		 (z > 205 && z <= zmax && std::abs(tree_y) > tan_beampipe_angle2*(z-205)+20.13)
																		 ) ) {
																				particle_went_outside_beampipe = true;
																}
																z = step*(zmax-zmin)/zbin;
																helix_positions = nullptr;
												}
												histo_x->FillN(zbin, z_array, x_array,nullptr,1);//number of entries in arrays, array for x, array for y, array for weights (if NULL then weight=1),step size through arrays
												histo_y->FillN(zbin, z_array, y_array,nullptr,1);
												if (particle_went_outside_beampipe == true){
																particles_outside_beampipe++;
												}
												else{
																particles_inside_beampipe++;
												}
								}
								file->Close();
				}

				std::cout << "-----------------" << std::endl;
				std::cout << "All particles drawn: " << particles_outside_beampipe + particles_inside_beampipe <<  std::endl;
				std::cout << "All particles outside the beam pipe: " << particles_outside_beampipe <<  std::endl;
				std::cout << "Ratio: "  << std::fixed << std::setprecision(3) << ((float)particles_outside_beampipe)/((float)(particles_outside_beampipe + particles_inside_beampipe)) <<  std::endl;
				std::cout << "-----------------" << std::endl;


				TLine *line = new TLine(0,12,62.5,12);
				TLine *nline = new TLine(0,-12,62.5,-12);
				TLine *line2 = new TLine(62.5,12,205,20.13);
				TLine *nline2 = new TLine(62.5,-12,205,-20.13);
				TLine *line3 = new TLine(205,20.13,300,28.99);
				TLine *nline3 = new TLine(205,-20.13,300,-28.99);
				line->SetLineColor(2);
				nline->SetLineColor(2);
				line2->SetLineColor(2);
				nline2->SetLineColor(2);
				line3->SetLineColor(2);
				nline3->SetLineColor(2);
				
				gStyle->SetOptStat(0);
				//gStyle->SetOptStat(111111);

				//Plot the histogram and save it
				TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
				canvas->cd();

				canvas->SetLogz();
				//histo_x->SetMinimum(1e-8);
				//histo_y->SetMinimum(1e-8);
				
				histo_x->Draw("colz");
				//canvas->Update();
				//TPaveStats *st = (TPaveStats*)histo->GetListOfFunctions()->FindObject("stats");
				//st->SetX1NDC(0.6); //new x start position
				//st->SetX2NDC(0.85); //new x end position
				//st->SetY1NDC(0.6); //new x start position
				//st->SetY2NDC(0.9); //new x end position

				line->Draw();
				nline->Draw();
				line2->Draw();
				nline2->Draw();
				line3->Draw();
				nline3->Draw();

				std::string histoname_x(histo_x->GetName());

				canvas->Print(("output/"+histoname_x+"_"+specialname+".pdf").c_str());
				canvas->Print(("output/"+histoname_x+"_"+specialname+".cxx").c_str());

				histo_y->Draw("colz");
				line->Draw();
				nline->Draw();
				line2->Draw();
				nline2->Draw();
				line3->Draw();
				nline3->Draw();

				std::string histoname_y(histo_y->GetName());

				canvas->Print(("output/"+histoname_y+"_"+specialname+".pdf").c_str());
				canvas->Print(("output/"+histoname_y+"_"+specialname+".cxx").c_str());
				
				Outputfile->Write();
				return 0;
}
