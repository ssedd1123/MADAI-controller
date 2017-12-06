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
	t_master.SetScales(1);
	t_master.SetNuggets(pow(10, -0.42));
	std::vector<Data> data = t_master.RunEmulator(t_para);
	std::vector<std::string> var_name;
	t_master.GetVarName(var_name);

	parameters << "S0 " << t_para[0] << "\nL " << t_para[1] << "\nms " << t_para[2] << "\nmv " << t_para[3] <<std::endl;
	
	for(unsigned int i = 0; i < data.size(); ++i)
		results << var_name[i] << " " << data[i].val << " " << data[i].err << "\n";
}


void ReplaceExpTrace(int t_no = 40)
{
	vector<vector<double>> para_list{ 	/*std::vector<double>{28, 50, 0.7, 0.8},
						std::vector<double>{34, 100, 0.9, 1.0},*/
						std::vector<double>{26.1841, 97.77, 0.898, 0.8495},
						/*std::vector<double>{33.2499, 58.664, 0.7724, 0.14875}*/};

	RunMaster controller("/projects/hira/MADAI/Tsangc/Ri35_Emulate/Ri35_simple", "Ri35_simple/");
	// configurations to be looped through
	std::vector<double> nuggets{/*0.001*/pow(10, -0.42)};
	std::vector<double> scales{/*0.01*/1};
	std::vector<int> excluded;
	for(int i = t_no; i < controller.GetEndNo(); ++i) excluded.push_back(i);

	RunMaster model("/projects/hira/MADAI/Tsangc/Ri35_2", "Ri35/");
	//model.SetSettings("EMULATOR_TRAINING_ALGORITHM", "exhaustive_geometric_kfold_common");

	int id = 0;
	for(auto it = para_list.begin(); it != para_list.end(); ++it, ++id)
		for(unsigned int j = 0; j < nuggets.size(); ++j)
		{
			std::cout << " Working on scales = " << scales[j] << " nuggets = " << nuggets[j] << " No run = " << t_no << "\n";
			controller.SetScales(scales[j]);
			controller.SetNuggets(nuggets[j]);
			CreateExpResult(model, *it);
			controller.SetExpResult("/projects/hira/MADAI/Tsangc/AnalysisEmulator/root_build/results.dat");
			controller.ExcludeRun(excluded);
			std::string directory = controller.GenerateTrace("NoRuns_" + std::to_string(t_no) + "_ID_" + std::to_string(id));
			std::cout << directory << endl;
			std::string command("cp /projects/hira/MADAI/Tsangc/AnalysisEmulator/root_build/parameters.dat " + directory);
			system(command.c_str());
		}
}
