#ifndef loglikelihood_H
#define loglikelihood_H
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
#include <thread>
#include <functional>


class loglikelihood
{
public:
	loglikelihood(RunMaster t_controller = RunMaster("/mnt/analysis/hira/tsangc/AnalysisEmulator/DataCreator", "DataCreator/")): controller_(t_controller)
	{};

	void Set(double scales, double nuggets)
	{
		controller_.SetNuggets(nuggets);
		controller_.SetScales(scales);
	}

	void ExpResult(const std::vector<double>& t_exp)
	{
		exp_result_ = t_exp;
	}

	void ExcludeRun(const std::vector<int>& t_list)
	{
		excluded_list = t_list;
		controller_.ExcludeRun(t_list);
	}

	double Eval(const std::vector<double>& t_para)
	{
		//std::clock_t start = std::clock();
		//std::vector<Data> data = controller_.RunEmulator(t_para);
		//std::cout << " loglikelihood finished running emulator " << ( std::clock() - start ) / (double) CLOCKS_PER_SEC << "\n";
		double logprob = controller_.GetLogLikelihood(t_para, exp_result_);
		//for(unsigned int u = 0; u < data.size(); ++u)
		//{
		//	double value = data[u].val;
		//	double error = data[u].err;
			/*if(data[u].err < 1e-4) 
			{
				std::vector<Data> error_dat = controller_.RunEmulator(vector<double>{t_para[0] + 0.01});
				error = error_dat[u].err;
			}*/
			//cout << " val and error " << exp_result_[u] << " " << error << endl;
		//	logprob += -pow((exp_result_[u] - value)/error, 2) - log((sqrt(2*M_PI)*error));
		//}
		// std::cout << " loglikelihood finished everything " << ( std::clock() - start ) / (double) CLOCKS_PER_SEC << "\n";	

		return logprob;
	};

	void Validation(double scale, double nugget, const std::vector<int>& t_validation_list = std::vector<int>{})
	{
		Set(scale, nugget);
		tot_logprob = 0;
		for(int i : ((t_validation_list.empty())? excluded_list : t_validation_list))
		{
			auto data = controller_.GetVarFromRun(i);
			std::vector<double> exp_result;
			for(auto val : data)
				exp_result.push_back(val.val);
			ExpResult(exp_result);
			tot_logprob += Eval(controller_.GetParFromRun(i));
		}
	};

	double GetLogProb(){return tot_logprob;};

	int GetEndNo(){return controller_.GetEndNo();};
	int GetStartNo(){return controller_.GetStartNo();};

	/*TFile *file = new TFile("chi2.root", "RECREATE");
	file->cd();
	th2f->Write("chi2");
	file->Close();*/
private:
	RunMaster controller_;
	std::vector<double> exp_result_;
	std::vector<int> excluded_list;
	double tot_logprob;
};
/*
class loglikelihood_multi
{
public:
	loglikelihood_multi()
	{
		list_calculate_.push_back(RunMaster("/mnt/analysis/hira/tsangc/AnalysisEmulator/DataCreator", "DataCreator/"));
		list_calculate_.push_back(RunMaster("/mnt/analysis/hira/tsangc/AnalysisEmulator/DataCreator2", "DataCreator/"));
		list_calculate_.push_back(RunMaster("/mnt/analysis/hira/tsangc/AnalysisEmulator/DataCreator3", "DataCreator/"));
		list_calculate_.push_back(RunMaster("/mnt/analysis/hira/tsangc/AnalysisEmulator/DataCreator4", "DataCreator/"));	
		list_calculate_.push_back(RunMaster("/mnt/analysis/hira/tsangc/AnalysisEmulator/DataCreator5", "DataCreator/"));
	};

	void ExcludeRun()
	{
		std::vector<vector<int>> excluded_list;
        	std::vector<int> temp;
        	int row_index = -1;
        	int partition = 21;
        	for(int i = 0; i < list_calculate_[0].GetEndNo() - list_calculate_[0].GetStartNo() + 1; ++i)
        	{
         	       if( i%partition == 0)
         	       {
         	               excluded_list.push_back(temp);
         	               ++row_index;
         	       }
         	       excluded_list[row_index].push_back(i + list_calculate_[0].GetStartNo());
       	 	}


		for(int i = 0; i < list_calculate_.size(); ++i)
			list_calculate_[i].ExcludeRun(excluded_list[i]);

	};

	double Validation(double scale, double nuggets)
	{
		std::vector<std::thread> jobs;
		double prob = 0;
		for(auto& calculate : list_calculate_ )
			jobs.emplace_back(&loglikelihood::Validation, std::ref(calculate), scale, nuggets);

		for(auto& t : jobs)
			t.join();

		for(auto& calculate : list_calculate_ )
			prob += calculate.GetLogProb();

		return prob;
	}
private:
	
	std::vector<loglikelihood> list_calculate_;
};

*/
#endif
