#include "ProcessPipe.h"
#include "test.h"
#include "TreeFiller.h"
#include "EmulatorInterface.h"
#include "DataSelector.h"
#include "RunInputReader.h"
#include "RunMaster.h"
#include "EmulatorConstants.h"
#include "CSVReader.h"
#include "SettingSetter.h"
#include "Data.h"
#include "TFile.h"
#include "TH2.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>
#include <regex>
#include <iomanip>
#include <regex>

#include "../src/TraceAnalyzer.cxx"
#include "../src/ExpResultSelector.cxx"
#include "../src/CSVReader.cxx"
#include "../src/DataSelector.cxx"
#include "../src/EmulatorInterface.cxx"
#include "../src/EmulatorOutputReader.cxx"
#include "../src/ProcessPipe.cxx"
#include "../src/RunInputReader.cxx"
#include "../src/SettingSetter.cxx"
#include "../src/RunMaster.cxx"


void CreateExpResult(RunMaster& t_master, const std::vector<double>& t_para, double t_noise)
{
	// generate noise in the signal
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(1 - t_noise, 1 + t_noise);

	t_master.SetSettings("EMULATOR_TRAINING_ALGORITHM", "exhaustive_geometric_kfold_common");
	std::ofstream results("results.dat");
	std::ofstream parameters("parameters.dat");
	std::vector<Data> data = t_master.RunEmulator(t_para);
	std::vector<std::string> var_name;
	t_master.GetVarName(var_name);

	parameters << "S0 " << t_para[0] << "\nL " << t_para[1] << "\nms " << t_para[2] << "\nfi " << t_para[3] <<std::endl;
	
	for(unsigned int i = 0; i < data.size(); ++i)
		results << var_name[i] << " " << data[i].val*dist(mt) << " " << sqrt(data[i].err*data[i].err + t_noise*t_noise) << "\n";
}


void GetMeanAndBest(RunMaster& controller, RunMaster& model, int t_no, std::vector<double>& t_par, std::vector<double>& t_mean, std::vector<double>& t_best, double t_scale, double t_nuggets, double t_noise = 0)
{
	
	// configurations to be looped through
	std::vector<int> excluded;
	for(int i = t_no; i < controller.GetEndNo(); ++i) excluded.push_back(i);

	std::cout << " Working on No run = " << t_no << " with noise " << t_noise << "\n";
	//controller.SetSettings("SAMPLER_NUMBER_OF_SAMPLES", "100000");
	controller.SetScales(/*0.6*/t_scale);
	controller.SetNuggets(/*pow(10, -2.1)*/t_nuggets);
	CreateExpResult(model, t_par, t_noise);
	controller.SetExpResult("/projects/hira/MADAI/Tsangc/AnalysisEmulator/root_build/results.dat");
	controller.ExcludeRun(excluded);
	controller.GenerateTrace("NoRuns_" + std::to_string(t_no) + "_AnalyzeNoNeeded", true, true);
	controller.GetTraceBest(t_best);
	controller.GetTraceMean(t_mean);
}

void DisplayVec(const std::vector<double>& t_vec)
{
	for(int i = 0; i < t_vec.size(); ++i) std::cout << t_vec[i] << " ";
	std::cout << "\n";
}

void AnalyzeNoNeeded()
{
	// record the results in a tree
	TFile *file = new TFile("No_Needed.root", "RECREATE");
	TTree *tree = new TTree("ttree","ttree");
	double scales, nuggets, noise;
	std::vector<double> mean, best, sd;
	std::vector<double> par{34, 50, 0.9, -0.4};
	cout << " Original parameters are ";
	DisplayVec(par);
	int no_run;
	tree->Branch("original", &par);
	tree->Branch("mean", &mean);
	tree->Branch("best", &best);	
	tree->Branch("no_run", &no_run);
	tree->Branch("scale", &scales);
	tree->Branch("SD", &sd);
	tree->Branch("nuggets", &nuggets);
	tree->Branch("noise", &noise);

	RunMaster controller("/projects/hira/MADAI/Tsangc/find_no_needed/find_no_needed", "find_no_needed/");
	RunMaster model("/projects/hira/MADAI/Tsangc/test_distribution/e120", "e120/");	
	for(noise = 0; noise <= 0.2; noise += 0.1)
		for(int i = 0; i < 3; ++i)
		{
			switch(i)
			{
				case 0:
					scales = 0.01;
					nuggets = 0.001;
					break;	
				case 1:
					scales = 0.6;
					nuggets = pow(10, -2.1);
					break;
				case 2:
					scales = 0.1;
					nuggets = 2;
					break;
			}
			for(double run = 40; run < 500; run +=100)
			{
				no_run = (int) run;
				GetMeanAndBest(controller, model, no_run, par, mean, best, scales, nuggets, noise);
				controller.GetTraceSD(sd);
				std::cout << " Best value ";
				DisplayVec(best);
				std::cout << " Mean value ";
				DisplayVec(mean);
				tree->Fill();
			}
		}

	file->Write();
	delete file;
}
