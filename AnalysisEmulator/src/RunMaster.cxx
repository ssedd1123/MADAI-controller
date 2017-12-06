#include "RunMaster.h"

//typedef std::chrono::high_resolution_clock Clock;

RunMaster::RunMaster(const std::string& t_path, const std::string& t_template) :
	template_(t_template),
	path_(t_path),
	start_no(-1),
	end_no(-1),
	no_par_(-1),
	no_var_(0),
	run_ready_(false),
	interface_(t_path, (SetRunNo(t_template), no_par_)),
	setter_(EmulatorConstants::path + "template/settings.dat.temp", t_path),
	exp_selector_(t_path, t_template),
	input_reader_(end_no + 1, RunInputReader(t_template)),
	selector_(t_path, t_template, start_no, end_no)
{
	//_interface.SetWorkingDir(t_path);
	for(int i = start_no; i <= end_no; ++i)
		input_reader_[i].LoadFile(i);
	no_var_ = input_reader_[start_no].GetVarNum();
	SetObsList();
}

void RunMaster::SetObsList(const std::vector<std::string>& t_obs_list)
{
	std::vector<std::string> original_obs_list;
	GetVarName(original_obs_list);

	std::ofstream obs_file((path_ + "/observable_names.dat").c_str());
	if(!obs_file.is_open()) throw std::runtime_error("RunMaster SetObsList cannot find observable_names.dat\n");
	obs_list_ = (t_obs_list.empty())? original_obs_list: t_obs_list;

	// fill out observable names if they are contained in the model_output
	for(auto&& name : obs_list_)
	{
		if(std::find(original_obs_list.begin(), original_obs_list.end(), name) != original_obs_list.end())
			obs_file << name << "\n";
		else throw std::runtime_error("RunMaster SetObsList some names in the given vector does not match what's given in the model output files\n");
	}

	no_var_ = obs_list_.size();

}

void RunMaster::SetExpResult(const std::vector<Data>& t_exp_result)
{
	std::string filepath(path_ + "/results.dat");
	std::ofstream results(filepath);
	if(!results.is_open()) throw std::runtime_error("SetExpResult fails to generate result files\n");
	if(t_exp_result.size() != no_var_) throw std::runtime_error("SetExpResult input vector is of wrong size\n");

	std::vector<std::string> var_name;
	input_reader_[start_no].GetVarName(var_name);
	for(unsigned int i = 0; i < no_var_; ++i)
		results << var_name[i] << " " << t_exp_result[i].val << " " << t_exp_result[i].err << "\n";

	SetExpResult(filepath);
	
}

void RunMaster::ExcludeRun(const std::vector<int>& t_run)
{
	excluded_run_ = t_run;
	run_ready_ = false;
}

void RunMaster::SetNuggets(double t_nuggets)
{
	if(t_nuggets <= 0) throw std::runtime_error("SetNuggets invalid nuggets (<0)"); 
	setter_.Set("EMULATOR_NUGGET", std::to_string(t_nuggets));
}

void RunMaster::SetScales(double t_scales)
{
	if(t_scales <= 0) throw std::runtime_error("SetScaless invalid scales (<0)");
	setter_.Set("EMULATOR_SCALE", std::to_string(t_scales));
}

void RunMaster::SetSettings(const std::string& t_setting_name, const std::string& t_setting_var)
{
	setter_.Set(t_setting_name, t_setting_var);
}

std::vector<double> RunMaster::GetParFromRun(int t_run_no)
{
	if(t_run_no < start_no) throw std::runtime_error("GetParFromRun run number invalid (<0)");
	std::vector<double> answer(no_par_);
	for(int i = 0; i < no_par_; ++i)
		answer[i] = input_reader_[t_run_no].GetPar(i);
	return answer;
}

std::vector<Data> RunMaster::GetVarFromRun(int t_run_no)
{
	if(t_run_no < start_no) throw std::runtime_error("GetVarFromRun run number invalid (<0)");
	std::vector<Data> answer(no_var_);
	for(int i = 0; i < no_var_; ++i)
	{
		answer[i].val = input_reader_[t_run_no].GetVar(i);
		answer[i].err = input_reader_[t_run_no].GetError(i);
	}
	return answer;
}

std::vector<Data> RunMaster::GetExpResult()
{
	RunInputReader exp_reader(template_);
	exp_reader.LoadRunFile(exp_selector_.GetExpPath());
	std::vector<Data> answer(no_var_);
	for(int i = 0; i < no_var_; ++i)
	{
		answer[i].val = exp_reader.GetVar(i);
		answer[i].err = exp_reader.GetError(i);
	}
	return answer;
}

double RunMaster::GetParFromRun(int t_run_no, int t_par_no)
{
	if(t_run_no < start_no) throw std::runtime_error("GetParFromRun run number invalid (<0)");
	if(t_par_no < 0) throw std::runtime_error("GetParFromRun par number invalid (<0)");	
	return input_reader_[t_run_no].GetPar(t_par_no);
}

Data RunMaster::GetVarFromRun(int t_run_no, int t_var_no)
{
	if(t_run_no < start_no) throw std::runtime_error("GetVarFromRun run number invalid (<0)");
	if(t_var_no < 0) throw std::runtime_error("GetParFromRun par number invalid (<0)");
	Data answer{0, 0};
	answer.val = input_reader_[t_run_no].GetVar(t_var_no);
	answer.err = input_reader_[t_run_no].GetError(t_var_no);
	return answer;
}

std::vector<Data> RunMaster::RunEmulator(const std::vector<double>& t_parameters, bool no_train, bool no_pca)
{
	if(t_parameters.size() != no_par_)
		throw std::runtime_error("RunMaster RunEmulator input parameters is of the wrong size\n");
	// check if the emulator needs to be trained again
	//auto start = Clock::now();
	//std::clock_t start = std::clock();
	bool need_training = false;
	bool need_pca = false;
	if(!run_ready_)
	{
		selector_.SingleOut(excluded_run_);
		current_excluded_run_ = excluded_run_;
		need_training = true;
		need_pca = true;
	}
	//auto end = Clock::now();
	//std::cout << " Finished excluding runs " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds \n";
	if(!setter_.ChangesApplied())
	{	
		setter_.ApplySetting(false);
		need_training = true;
	}
	//end = Clock::now();
	//std::cout << " Finished changing setings " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds \n";
	if(!no_train && need_training) 
	{
		interface_.TrainEmulator(need_pca && !no_pca);
	}
	//end = Clock::now();
	//std::cout << " Finished training " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds \n";
	// reset flages 
	run_ready_ = true;
	std::vector<Data> data(no_var_);
	interface_.RunEmulator(t_parameters);
	for(int i = 0; i < no_var_; ++i)
	{
		data[i].val = interface_.GetVar(i);
		data[i].err = sqrt(fabs(interface_.GetCovMatrix(i, i)));
	}
	//end = Clock::now();
	//std::cout << " Finished running " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds \n";
	return data;	
}

double RunMaster::GetLogLikelihood(const std::vector<double>& t_parameters, const std::vector<double>& t_for_value)
{
	//double scale_up = 1e6;
	if(t_for_value.size() != no_var_)
		throw std::runtime_error("RunMaster GetLogLikelihood for value is of the wrong size\n");
	RunEmulator(t_parameters);
	//TMatrixDSym error;
	//error.ResizeTo(no_var_, no_var_);
	//TVectorD value;
	//value.ResizeTo(no_var_);
        double log_det = 0;
	double tot_similarity = 0;
	for(int i = 0; i < no_var_; ++i)
	{
		double value = interface_.GetVar(i) - t_for_value[i];
                double error = interface_.GetCovMatrix(i, i);
		// temporary hack to get around the fact that some points may have absolutely no variance
		// those are points used for normalization and their value is held constant
		// techniquely speaking since it is used in normalization
		// experimental value should match exactly
		// therefore it shouldn't have affacted the result
		error = (error < 1e-16)? 0.1: error;
                log_det += log(error);
		tot_similarity += value*value/error;
		//for(int j = 0; j < no_var_; ++j)
		//	error[i][j] = (i == j)? interface_.GetCovMatrix(i, i) : 0;
	}
	/*for(auto par : t_parameters)
		cout << par << " ";
	cout << endl;
	error.Print();
	
	TDecompChol chol(error);

	double d1, d2;
	chol.Det(d1, d2);
	cout << d1*pow(2., d2) << endl;
	
	throw std::runtime_error("please remove this line");
	for(int i = 0; i < no_var_; ++i)
	{
		for(int j = 0; j < no_var_; ++j)
			error[i][j] /= scale_up;
	}*/

	//double determinant;
	//error.Invert(&determinant);
	return -0.5*log_det - 0.5*tot_similarity - 0.5*log(2.*M_PI);
	
}


std::string RunMaster::GenerateTrace(const std::string& t_option, bool t_analyze, bool t_del_after_analyze)
{
	bool need_training = false;
	if(!run_ready_)
	{
		selector_.SingleOut(excluded_run_);
		current_excluded_run_ = excluded_run_;
		need_training = true;
	}
	if(!setter_.ChangesApplied())
	{	
		setter_.ApplySetting(false);
		need_training = true;
	}
	/*if(need_training)*/ interface_.TrainEmulator();

	run_ready_ = true;
	// create folders with its name showing the setting changes
	std::vector<std::string> setting_name, setting_var;
	setter_.GetListOfChanges(setting_name, setting_var);
	std::string folder_name = "CSV_" + template_.substr(0, template_.size() - 1);
	for(unsigned int i = 0; i < setting_name.size(); ++i)
	{
		folder_name = folder_name + "_" + setting_name[i];
		folder_name = folder_name + "_" + setting_var[i];
	}
	folder_name = folder_name + "_" + t_option;
        std::string just_name = folder_name;
        folder_name = EmulatorConstants::path + "/CSV_output/" + folder_name;
	std::string command = "mkdir -p " + folder_name;
	system(command.c_str());
	std::string csv_full_path = folder_name + "/" + template_.substr(0, template_.size() - 1) + ".csv";
	command = "madai_generate_trace " + path_ + " " + csv_full_path;
	system(command.c_str());

	// store setting.dat into the same folder just to be sure
	command = "/bin/cp " + path_ + "/settings.dat " + folder_name + "/settings.dat";
	system(command.c_str());
	//setter_.Reset();

	if(t_analyze)
	{
		analyzer_.AnalyzeTrace(csv_full_path, path_);
		if(t_del_after_analyze) system(("rm -f " + csv_full_path).c_str());
	}else
	{
		// record details of the file with the file tagging system
                std::string prefix_command = "python /user/tsangc/python/changeTag.py \"" + EmulatorConstants::path + "CSV_output\" \"" + just_name + "\"";
                for(unsigned int i = 0; i < setting_name.size(); ++i)
                {
			command = prefix_command + " \"" + setting_name[i] + "\" \"" + setting_var[i] + "\"";
                        system(command.c_str());
                }
		// number of runs excluded
                command = "[";
		for(auto&& i : current_excluded_run_)
			command += to_string(i) + ",";
		command.pop_back();
		if(!command.empty())
		{
                	command += "]";
			command = prefix_command + " \"Excluded_Run\" \"" + command + "\"";
			system(command.c_str()); 
		}

		// exp result used
		command = exp_selector_.GetExpPath();
		command = prefix_command + " \"Exp_result\" \"" + command + "\"";
		system(command.c_str());
		
		// template used
		command = prefix_command + " \"Type\" \"" + template_ + "\"";
                system(command.c_str());
	}

	return folder_name;
}

void RunMaster::SetRunNo(const std::string& t_template)
{
	// run only if the numbers are not initialized
	if(no_par_ < 0 || start_no < 0 || end_no < 0)
	{
		std::string command(EmulatorConstants::path + "src/RunNumber.sh " + EmulatorConstants::path + "template/model_output/" + t_template);
		ProcessPipe getnumber(command);
		// will be used again to find number of parameters
		start_no = atoi(getnumber.GetOutput().c_str());
		end_no = atoi(getnumber.GetOutput().c_str());
		no_par_ = atoi(getnumber.GetOutput().c_str());

	}

}

void RunMaster::AnalyzeTrace(const std::string& t_csv)
{
	analyzer_.AnalyzeTrace(t_csv, path_);
}
