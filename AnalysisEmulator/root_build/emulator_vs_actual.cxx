#include "ProcessPipe.h"
#include "test.h"
//#include "EmulatorOutputReader.h"
#include "EmulatorInterface.h"
#include "DataSelector.h"
#include "RunInputReader.h"
#include "RunMaster.h"
#include "EmulatorConstants.h"
#include "CSVReader.h"
#include "SettingSetter.h"
#include "Data.h"
#include "TFile.h"
#include "TTree.h"
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

class TreeFiller
{
public:
	TreeFiller(const std::string& t_filename) : file_(new TFile(t_filename.c_str(), "RECREATE")){};
	~TreeFiller() 
	{ 
		file_->cd(); 	
		for(unsigned int i = 0; i < tree_.size(); ++i)
			tree_[i]->Write(tree_name_[i].c_str(), TObject::kOverwrite);
	};

	// return the tree index of the added tree for access
	int AddTree(const std::string& t_tree_name)
	{ 
		tree_name_.push_back(t_tree_name); 
		tree_.push_back(CreateTree());
		return tree_.size() - 1;
	};

	void FillTree(int t_tree_id, int t_run_no, double t_scales, double t_nuggets, const std::vector<Data> t_emulator, const std::vector<Data> t_actual)
	{
		scales_ = t_scales;
		nuggets_ = t_nuggets;
		run_no_ = t_run_no;
		unsigned int para_no = t_emulator.size();
		for(var_index_ = 0; var_index_ < para_no; ++var_index_)
		{
			emulator_result_val_ = t_emulator[var_index_].val;
			emulator_result_err_ = t_emulator[var_index_].err;
			actual_result_val_ = t_actual[var_index_].val;
			actual_result_err_ = t_actual[var_index_].err;
			tree_[t_tree_id]->Fill();
		}
	}
	
private:
	TTree* CreateTree()
	{
		TTree *tree = new TTree;
		tree->Branch("var_index", &var_index_);
		tree->Branch("run_no", &run_no_);
		tree->Branch("scales", &scales_);
		tree->Branch("nuggets", &nuggets_);
		tree->Branch("emulator_result_val", &emulator_result_val_);
		tree->Branch("emulator_result_err", &emulator_result_err_);
		tree->Branch("actual_result_val", &actual_result_val_);
		tree->Branch("actual_result_err", &actual_result_err_);
		return tree;
	};

	TFile *file_;
	std::vector<TTree*> tree_;
	std::vector<std::string> tree_name_;

	unsigned int var_index_;
	int run_no_;
	double scales_;
	double nuggets_;
	double emulator_result_val_;
	double emulator_result_err_;
	double actual_result_val_;
	double actual_result_err_;
};

std::vector<Data> emulator_result(RunMaster& t_controller, int t_run)
{
	std::vector<int> excluded;
	if(t_run <= 10) excluded = std::vector<int>{1,2,3,4,5,6,7,8,9,10};
	else if (t_run <= 20) excluded = std::vector<int>{11,12,13,14,15,16,17,18,19,20};
	else if (t_run <= 30) excluded = std::vector<int>{21,22,23,24,25,26,27,28,29,30};
	else if (t_run <= 40) excluded = std::vector<int>{31,32,33,34,35,36,37,38,39,40};
	else if (t_run <= 49) excluded = std::vector<int>{41,42,43,44,45,46,47,48,49};
	t_controller.ExcludeRun(std::vector<int>{t_run});
	return t_controller.RunEmulator(t_controller.GetParFromRun(t_run));
}

std::vector<Data> actual_result(RunMaster& t_controller, int t_run)
{
	return t_controller.GetVarFromRun(t_run);
}

void emulator_vs_actual()
{
	RunMaster controller("/projects/hira/MADAI/Tsangc/test_distribution2/e120", "e120/");
	controller.SetNuggets(0.001);
	controller.SetScales(0.01);

	TreeFiller treefiller("test.root");

	std::vector<double> nuggets{0.001, 0.839};
	std::vector<double> scales{0.01, 0.35};

	// loop through all possible combination of settings
	for(int i = 0 ; i < nuggets.size() + 1; ++i)
//		for(int mcmc_error = 0; mcmc_error < 2 ; ++mcmc_error)
//			for(int pca_error = 0; pca_error < 2; ++pca_error)
			{
				std::cout << " Generating tree for :\n";
				if(i == nuggets.size()) 
				{
					controller.SetSettings("EMULATOR_TRAINING_ALGORITHM", "exhaustive_geometric_kfold_common");
					std::cout << " exhaustive_geometric_kfold_common\n";
				}
				else
				{
					controller.SetNuggets(nuggets[i]);
					controller.SetScales(scales[i]);
					std::cout << " Scales " << scales[i] << " Nuggets " << nuggets[i] << "\n";
				}
/*				if(mcmc_error == 0) 
				{
					controller.SetSettings("MCMC_USE_MODEL_ERROR", "1");
					std::cout << " mcmc use model error\n";
				}
				if(pca_error == 0) 
				{
					controller.SetSettings("PCA_USE_MODEL_ERROR", "1");
					std::cout << " pca use model error\n";
				}
*/

				// set tree name
				std::string tree_name = "";
				std::vector<std::string> listOfChangesName, listOfChangesValue;
				controller.GetListOfChanges(listOfChangesName, listOfChangesValue);
				for(unsigned int j = 0; j < listOfChangesName.size(); ++j)
					tree_name += listOfChangesName[j] + "_" + listOfChangesValue[j] + "_";


				int tree_index = treefiller.AddTree(tree_name);
				for(int j = EmulatorConstants::start_run; j < EmulatorConstants::start_run + EmulatorConstants::no_run; ++j)
					treefiller.FillTree(tree_index, j, scales[i], nuggets[i], emulator_result(controller, j), actual_result(controller, j));

				controller.Reset();
			}

}
