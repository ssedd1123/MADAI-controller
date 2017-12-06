#ifndef RUNMASTER_H
#define RUNMASTER_H
#include "EmulatorInterface.h"
#include "EmulatorConstants.h"
#include "RunInputReader.h"
#include "DataSelector.h"
#include "SettingSetter.h"
#include "ExpResultSelector.h"
#include "TraceAnalyzer.h"
#include "Data.h"
#include "TMatrixDSym.h"
#include "TVectorD.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <ctime>

class RunMaster
{
public:
	RunMaster(const std::string&, const std::string&);
	~RunMaster(){};
	void SetObsList(const std::vector<std::string>& t_obs_list = std::vector<std::string>(0));
	int GetStartNo() { return start_no; };
	int GetEndNo() { return end_no; };
	void ExcludeRun(const std::vector<int>&);
	void SetNuggets(double);
	void SetScales(double);
	void SetSettings(const std::string&, const std::string&);
        void SetExpResult(const std::string& t_filename) 
	{
		// search and see if file in the template region exist first, than search for global space
		std::string filename =(EmulatorConstants::path + "template/experimental_output/" + template_ + t_filename); 
		std::ifstream infile(filename.c_str());
		if(infile.good())
		{
			infile.close();
			cout << filename << endl;
			exp_selector_.SetExpResult(filename);
		} 
		else exp_selector_.SetExpResult(t_filename);
	};
	void SetExpResult(int i){ SetExpResult(GetDataDir(i) + "/results.dat");};
	void SetExpResult(const std::vector<Data>&);
        std::string GetDataDir(int i) { return selector_.GetDataDir(i);};
	std::vector<double> GetParFromRun(int);
	std::vector<Data> GetVarFromRun(int);
	std::vector<Data> GetExpResult();
	double GetParFromRun(int, int);
	Data GetVarFromRun(int, int);
	std::vector<Data> RunEmulator(const std::vector<double>&, bool no_train = false, bool no_pca = false);
	double GetLogLikelihood(const std::vector<double>&, const std::vector<double>&);
	std::vector<int> GetExcludedRun() { return current_excluded_run_; };
	std::string GenerateTrace(const std::string& t_option = "", bool t_analyze = false, bool t_del_after_analyze = false); // return the directory of the folder
	std::string GetTemplate() { return template_;};
	void GetListOfChanges(	std::vector<std::string>& t_setting_name, 
				std::vector<std::string>& t_setting_var)
		{ setter_.GetListOfChanges(t_setting_name, t_setting_var);};
	void Reset() { setter_.Reset();};
	void GetParName(std::vector<std::string>& t_vec){ input_reader_[start_no].GetParName(t_vec); };
	void GetVarName(std::vector<std::string>& t_vec){ input_reader_[start_no].GetVarName(t_vec); };

	void AnalyzeTrace(const std::string&); // manually input the directory of the csv file
	void GetTraceMean(std::vector<double>& t_vec) { analyzer_.GetParMean(t_vec);};
	void GetTraceBest(std::vector<double>& t_vec) { analyzer_.GetParBest(t_vec);};
	void GetTraceBestLikelihood(std::vector<double>& t_vec) { analyzer_.GetParBestLikelihood(t_vec);};
	void GetTraceSD(std::vector<double>& t_vec) { analyzer_.GetParSD(t_vec);};
	void GetTraceScaledDev(std::vector<double>& t_vec) { analyzer_.GetParScaledDev(t_vec);};
	//double GetNuggets() { return current_nuggets_; };
	//double GetScales() { return current_scales_; };
private:
	std::string template_;
	std::string path_;
	int start_no, end_no;
	// number of parameters for this run
	int no_par_;
	int no_var_;
	EmulatorInterface interface_;
	SettingSetter	setter_;
	ExpResultSelector exp_selector_;
	std::vector<RunInputReader> input_reader_; // 1 reader for each run
	DataSelector selector_;
	TraceAnalyzer analyzer_;
	// varables that stores the current configuration of the emulator
	std::vector<int> current_excluded_run_;
	// variables that needs to be set
	std::vector<int> excluded_run_;
	// see if the parameters are outdated
	bool run_ready_;
	std::vector<std::string> obs_list_;

	void SetRunNo(const std::string& t_template);
};

#endif
