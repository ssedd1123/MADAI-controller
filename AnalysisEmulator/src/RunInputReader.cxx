#include "RunInputReader.h"

void RunInputReader::LoadFile(int t_no)
{
	// generate the path of model run files
	std::stringstream run_path;
	run_path << EmulatorConstants::path + "template/model_output/" + template_ +"run";
	run_path << std::setw(4) << std::setfill('0') << t_no;

	var_name_.clear();
	var_to_index_.clear();
	var_value_.clear();
	var_error_.clear();
	
	par_name_.clear();
	par_to_index_.clear();
	par_value_.clear();


        // load result file
	std::string results_name = run_path.str() + "/results.dat";
	LoadRunFile(results_name);
	std::string parameters_name = run_path.str() + "/parameters.dat";
	LoadParFile(parameters_name);
}

void RunInputReader::LoadRunFile(const std::string& t_results_name)
{
        std::ifstream run_input(t_results_name.c_str());
        if(!run_input.is_open())
                throw std::runtime_error("LoadFile File failed to opened");
        std::string line;
	int name_index = 0;
        while(std::getline(run_input, line))
        {
		std::string name, value, error;
		std::stringstream ss(line);
		ss >> name >> value >> error;
		var_name_.push_back(name);
		var_to_index_.insert(std::make_pair(name, name_index));
		var_value_.push_back(atof(value.c_str()));
		var_error_.push_back(atof(error.c_str()));
         	++name_index;       
        }
	var_num_ = name_index;
        run_input.close();
}

void RunInputReader::LoadParFile(const std::string& t_parameters_name)
{
	// load parameters file	
        std::ifstream run_parameters(t_parameters_name.c_str());
        if(!run_parameters.is_open())
                throw std::runtime_error("LoadFile File failed to opened");
	std::string line;
	int name_index = 0;
        while(std::getline(run_parameters, line))
        {
		std::string name, value;
		std::stringstream ss(line);
		ss >> name >> value;
		par_name_.push_back(name);
		par_to_index_.insert(std::make_pair(name, name_index));
		par_value_.push_back(atof(value.c_str()));
         	++name_index;       
        }
	par_num_ = name_index;
        run_parameters.close();
}

void RunInputReader::GetVarName(std::vector<std::string>& t_input)
{
        t_input.resize(var_num_);
        for(int i = 0; i < var_num_; ++i) 
		t_input[i] = var_name_[i];
}

int RunInputReader::GetVarIndex(const std::string& t_input)
{
        std::map<std::string, int>::iterator it = var_to_index_.find(t_input.c_str());
        if(it == var_to_index_.end()) throw std::runtime_error("GetVarIndex variable name not found");
        return it->second;
}

double RunInputReader::GetVar(unsigned int t_i)
{
        if(t_i > var_value_.size()) throw std::runtime_error("GetVar index out of range");
        return var_value_[t_i];
}

double RunInputReader::GetError(unsigned int t_i)
{
        // it is triangular matrix, it should be fine
        if(t_i > var_error_.size()) throw std::runtime_error("GetError index out of range");
        return var_error_[t_i];
}

void RunInputReader::GetParName(std::vector<std::string>& t_input)
{
        t_input.resize(par_num_);
        for(int i = 0; i < par_num_; ++i) t_input[i] = par_name_[i];
}

int RunInputReader::GetParIndex(const std::string& t_input)
{
        std::map<std::string, int>::iterator it = par_to_index_.find(t_input.c_str());
        if(it == par_to_index_.end()) throw std::runtime_error("GetVarIndex pariable name not found");
        return it->second;
}

double RunInputReader::GetPar(unsigned int t_i)
{
        if(t_i > par_value_.size()) throw std::runtime_error("GetVar index out of range");
        return par_value_[t_i];
}

int RunInputReader::GetVarNum()
{
	if(var_num_ == 0) throw std::runtime_error("GetVarNum return 0 variables. Did you load the file first?");
	return var_num_;
}

int RunInputReader::GetParNum()
{
	if(par_num_ == 0) throw std::runtime_error("GetParNum return 0 variables. Did you load the file first?");
	return par_num_;
}
