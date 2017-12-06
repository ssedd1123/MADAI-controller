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


void CreateExpResult(RunMaster& t_master, const std::vector<double>& t_para)
{
	std::ofstream results("results.dat");
	std::ofstream parameters("parameters.dat");
	std::vector<Data> data = t_master.RunEmulator(t_para);
	std::vector<std::string> var_name;
	t_master.GetVarName(var_name);

	parameters << "S0 " << t_para[0] << "\nL " << t_para[1] << "\nms " << t_para[2] << "\nfi " << t_para[3] <<std::endl;
	
	for(unsigned int i = 0; i < data.size(); ++i)
		results << var_name[i] << " " << data[i].val << " " << data[i].err << "\n";
}


void RunOnItself(int t_no = 40)
{
	double S0 =30, L= 80, ms = 0.8, fI= 0;
	vector<vector<double>> para_list{ 	std::vector<double>{28, 50, 0.7, -0.4},
						std::vector<double>{34, 100, 0.9, 0.4},
						std::vector<double>{28, 100, 0.7, 0.4},
						std::vector<double>{34, 50, 0.9, -0.4}};

	// configurations to be looped through
	std::vector<double> nuggets{/*0.001*/pow(10, -2.1)};
	std::vector<double> scales{/*0.01*/0.6};
	std::vector<int> excluded;

	RunMaster model("/projects/hira/MADAI/Tsangc/test_distribution/e120", "e120/");
	model.SetSettings("EMULATOR_TRAINING_ALGORITHM", "exhaustive_geometric_kfold_common");

	int id = 0;
	for(auto it = para_list.begin(); it != para_list.end(); ++it, ++id)
		{
			model.SetSettings("EMULATOR_TRAINING_ALGORITHM", "exhaustive_geometric_kfold_common");
			CreateExpResult(model, *it);
			model.SetExpResult("/projects/hira/MADAI/Tsangc/AnalysisEmulator/root_build/results.dat");
			model.SetSettings("EMULATOR_TRAINING_ALGORITHM", "exhaustive_geometric_kfold_common");
			std::string directory = model.GenerateTrace("Itself_ID_" + std::to_string(id));
			std::cout << directory << endl;
			std::string command("cp /projects/hira/MADAI/Tsangc/AnalysisEmulator/root_build/parameters.dat " + directory);
			system(command.c_str());
		}
}
