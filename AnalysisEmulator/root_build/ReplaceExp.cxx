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

#include "../src/ExpResultSelector.cxx"
#include "../src/CSVReader.cxx"
#include "../src/DataSelector.cxx"
#include "../src/EmulatorInterface.cxx"
#include "../src/EmulatorOutputReader.cxx"
#include "../src/ProcessPipe.cxx"
#include "../src/RunInputReader.cxx"
#include "../src/SettingSetter.cxx"
#include "../src/RunMaster.cxx"

std::vector<Data> ReplaceExp(RunMaster& t_master, double t_scale, double t_nuggets, int t_excluded)
{
	std::cout << " Working on scales = " << t_scale << " nuggets = " << t_nuggets << " Replaced run = " << t_excluded << "\n";
	std::string option = "_Excluded_Run_" + std::to_string(t_excluded);
	t_master.SetScales(t_scale);
	t_master.SetNuggets(t_nuggets);
	t_master.SetExpResult(t_excluded);
	t_master.ExcludeRun(std::vector<int>{t_excluded});
	//t_master.GenerateTrace(option);
	return t_master.RunEmulator(t_master.GetParFromRun(t_excluded));
}

std::vector<Data> ReplaceExp(RunMaster& t_master, int t_excluded)
{
	std::cout << " Working on exhaustive geometric kfold Replaced run = " << t_excluded << "\n";
	std::string option = "_Excluded_Run_" + std::to_string(t_excluded);
	t_master.SetScales(1);
	t_master.SetNuggets(1);
	t_master.SetSettings("EMULATOR_TRAINING_ALGORITHM", "exhaustive_geometric_kfold_common");
	t_master.SetExpResult(t_excluded);
	t_master.ExcludeRun(std::vector<int>{t_excluded});
	//t_master.GenerateTrace(option);
	return t_master.RunEmulator(t_master.GetParFromRun(t_excluded));
}

void ReplaceExp()
{
	RunMaster controller("/projects/hira/MADAI/Tsangc/find_no_needed/find_no_needed", "find_no_needed/");
	// configurations to be looped through
	std::vector<double> nuggets{0.001, 0.839, 0.001};
	std::vector<double> scales{0.01, 0.35, 2};
	std::vector<int> excluded{1, 5, 10};

	// fill the emulator output and actual output to a tree
	TFile *file = new TFile("Result_replaceexp_find_no_needed.root", "RECREATE");
	TTree *tree = new TTree;
	double actual_result_val, actual_result_err, emulator_result_val, emulator_result_err;
	double scales_write, nuggets_write;
	int var_no, excluded_run;
	tree->Branch("actual_result_val", &actual_result_val);
	tree->Branch("actual_result_err", &actual_result_err);
	tree->Branch("emulator_result_val", &emulator_result_val);
	tree->Branch("emulator_result_err", &emulator_result_err);
	tree->Branch("scales", &scales_write);
	tree->Branch("nuggets", &nuggets_write);
	tree->Branch("var_no", &var_no);
	tree->Branch("excluded_run", &excluded_run);

	int j = 1, i = 2;
	for(unsigned int i = 0; i < excluded.size(); ++i)
	{
		for(unsigned int j = 0; j < nuggets.size(); ++j)
		{
			std::vector<Data> data_emu = ReplaceExp(controller, scales[j], nuggets[j], excluded[i]);
			std::vector<Data> data_act = controller.GetVarFromRun(excluded[i]);
			for(unsigned int k = 0; k < data_emu.size(); ++k)
			{
				emulator_result_val = data_emu[k].val;
				emulator_result_err = data_emu[k].err;
				actual_result_val = data_act[k].val;
				actual_result_err = data_act[k].err;
				scales_write = scales[j];
				nuggets_write = nuggets[j];
				var_no = k;
				excluded_run = excluded[i];
				tree->Fill();
			}
		}

		/*std::vector<Data> data_emu = ReplaceExp(controller, excluded[i]);
		std::vector<Data> data_act = controller.GetVarFromRun(excluded[i]);
		for(unsigned int k = 0; k < data_emu.size(); ++k)
		{
			emulator_result_val = data_emu[k].val;
			emulator_result_err = data_emu[k].err;
			actual_result_val = data_act[k].val;
			actual_result_err = data_act[k].err;
			scales_write = -1;
			nuggets_write = -1;
			var_no = k;
			excluded_run = excluded[i];
			tree->Fill();
		}*/
	}

	tree->Write("tree", TObject::kOverwrite);
}
