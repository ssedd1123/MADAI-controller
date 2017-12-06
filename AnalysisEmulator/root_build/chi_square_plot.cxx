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


void push_back_log(std::vector<double>& t_vec, double t_min, double t_max, int t_no)
{
        double no = (double) t_no;
        double factor = pow(std::exp(1), log(t_max/t_min)/no);
        for(double p = t_min; p <= t_max; p *= factor)
                t_vec.push_back(p);
}

void push_back_lin(std::vector<double>& t_vec, double t_min, double t_max, int t_no)
{
        double no = (double) t_no;
        double factor = (t_max - t_min)/no;
        for(double p = t_min; p <= t_max; p += factor)
                t_vec.push_back(p);
}


void fill_data(TreeFiller& filler, RunMaster& controller, double t_nuggets, double t_scales, const std::vector<int>& t_excluded)
{
	double chi2tot = 0;
	double log_prob_sum = 0;

	controller.SetNuggets(t_nuggets);
	controller.SetScales(t_scales);
	for(unsigned int k = 0; k < t_excluded.size(); ++k)
	{
		std::vector<Data> data = controller.RunEmulator(controller.GetParFromRun(t_excluded[k]));
		std::vector<Data> from_run = controller.GetVarFromRun(t_excluded[k]);
		filler.FillTree(0, t_excluded[k], t_scales, t_nuggets, data, from_run);
		//double log_prob = 0;
		/*for(unsigned int u = 0; u < data.size(); ++u)
		{
			Data from_run = controller.GetVarFromRun(t_excluded[k], u);
			log_prob += -log(data[u].err) - 0.5*pow((from_run.val - data[u].val)/data[u].err, 2) - 0.5*log(2*M_PI);
		}
		log_prob_sum += log_prob;*/
	}


	//return log_prob_sum;
}

void chi_square_plot()
{
	// plot settings
	double scales_min = 1e-3;
	double scales_max = 1.;
	bool scales_log = false;
	int scales_num = 15;
	double nuggets_min = 1e-3;
	double nuggets_max = 10;
	bool nuggets_log = false;//true;
	int nuggets_num = 15;

	std::vector<double> nuggets;
	std::vector<double> scales;

	// calculate interval size
	if(nuggets_log) push_back_log(nuggets, nuggets_min, nuggets_max, nuggets_num);
	else push_back_lin(nuggets, nuggets_min, nuggets_max, nuggets_num);
	//nuggets.push_back(0.5);

	if(scales_log) push_back_log(scales, scales_min, scales_max, scales_num);
	else push_back_lin(scales, scales_min, scales_max, scales_num);

	RunMaster controller("/mnt/analysis/hira/tsangc/AnalysisEmulator/DataCreator", "DataCreator/"); //"/projects/hira/MADAI/Tsangc/TestModelEmulate/TestModelEmulate", "TestModelEmulate/");

	// list of excluded runs
	std::vector<std::vector<int>> excluded_list;
	std::vector<int> temp;
	int row_index = -1;
	int partition = 25;
	for(int i = 0; i < controller.GetEndNo() - controller.GetStartNo() + 1; ++i)
	{
		if( i%partition == 0)
		{
			excluded_list.push_back(temp);
			++row_index;
		}
		excluded_list[row_index].push_back(i + controller.GetStartNo());
	}

	// TH2F to plot all the result
	TreeFiller treefiller("Training_Demonstrate.root");
	treefiller.AddTree("training");

	for(unsigned int i = 0; i < excluded_list.size(); ++i)
	{
		controller.ExcludeRun(excluded_list[i]);
		for(auto it_scales = scales.begin(); it_scales != scales.end(); ++it_scales)
			for(auto it_nuggets = nuggets.begin(); it_nuggets != nuggets.end(); ++it_nuggets)
			{
				std::cout << "\r" << "Processing chi square with nuggets = " << std::scientific
				<< std::setw(10) << std::setfill('0') << *it_nuggets 
				<< " and scales = " 
				<< std::setw(10) << std::setfill('0') << *it_scales << " For excluded list " << std::setw(3) << std::setfill('0') << i << std::flush;

				fill_data(treefiller, controller, *it_nuggets, *it_scales, excluded_list[i]);
			}
	}

	/*TFile *file = new TFile("chi2.root", "RECREATE");
	file->cd();
	th2f->Write("chi2");
	file->Close();*/
}


