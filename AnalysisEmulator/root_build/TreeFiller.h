#ifndef TREEFILLER_H
#define TREEFILLER_H
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

	void FillTree(int t_tree_id, int t_run_no, double t_scales, double t_nuggets, const std::vector<Data> t_emulator, const std::vector<Data> t_emulator_error, const std::vector<Data> t_actual)
        {
                scales_ = t_scales;
                nuggets_ = t_nuggets;
                run_no_ = t_run_no;
                unsigned int para_no = t_emulator.size();
                for(var_index_ = 0; var_index_ < para_no; ++var_index_)
                {
                        emulator_result_val_ = t_emulator[var_index_].val;
                        emulator_result_err_ = t_emulator_error[var_index_].err;
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

#endif
