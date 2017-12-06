#include "DataCreator.h"

	// input are vectors of parameters name and variable names respectively
DataCreator::DataCreator(const std::vector<std::string>& t_par_name, const std::vector<std::string>& t_var_name):
	par_name_(t_par_name),
	var_name_(t_var_name),
	par_num_(t_par_name.size()),
	var_num_(t_var_name.size()),
	par_upper_limit_(t_par_name.size(), - std::numeric_limits<double>::infinity()),
	par_lower_limit_(t_par_name.size(), std::numeric_limits<double>::infinity())
{}

void DataCreator::PushBack(const std::vector<double>& t_par, const std::vector<double>& t_var, const std::vector<double>& t_var_error)
{
	if(t_par.size() != par_num_)
		throw std::out_of_range("DataCreator PushBack parameter size not matched!\n");
	if(t_var.size() != var_num_)
		throw std::out_of_range("DataCreator PushBack variable size not matched!\n");

	var_list_.push_back(t_var);
	var_error_list_.push_back(t_var_error);
	par_list_.push_back(t_par);
	SetLimit(t_par);
}

void DataCreator::SetPoint(unsigned int t_i, const std::vector<double>& t_par, const std::vector<double>& t_var, const std::vector<double>& t_var_error)
{
	if(t_par.size() != par_num_)
		throw std::out_of_range("DataCreator SetPoint parameter size not matched!\n");
	if(t_var.size() != var_num_)
		throw std::out_of_range("DataCreator SetPoint variable size not matched!\n");
	if(t_i >= par_list_.size())
		throw std::out_of_range("DataCreator SetPoint number of event out of range!\n");

	var_list_[t_i] = t_var;
	var_error_list_[t_i] = t_var_error;
	par_list_[t_i] = t_par;
	SetLimit(t_par);
}

void DataCreator::SetLimit(const std::vector<double>& t_par)
{
	for(unsigned int i = 0; i < t_par.size(); ++i)
	{
		if(t_par[i] > par_upper_limit_[i]) par_upper_limit_[i] = t_par[i];
		if(t_par[i] < par_lower_limit_[i]) par_lower_limit_[i] = t_par[i];
	}
} 

RunMaster DataCreator::GetController()
{
	if(par_list_.size() == 0)
		throw std::runtime_error("DataCreator GetController You need to push back data first\n");
	
	// remove any existing file
	system(("rm -rf " + data_destination_ + "/run*").c_str());

	// create files for training
	for(unsigned int i = 0; i < par_list_.size(); ++i)
	{
		// stringstream for foldername
		std::stringstream ss;
		ss << "/run" << std::setw(4) << std::setfill('0') << i;
		std::string folder = data_destination_ + ss.str();
		system(("mkdir " + folder).c_str());

		// create variable files
		std::ofstream var_file((folder + "/" + var_filename_).c_str());
		if(!var_file.is_open()) 
			throw std::runtime_error("DataCreator GetController Cannot create variable files.\n");
	
		for(unsigned int j = 0; j < var_list_[i].size(); ++j)
			var_file << var_name_[j] << " " << var_list_[i][j] << " " << var_error_list_[i][j] << "\n";
		var_file.close();

		// create parameter files
		std::ofstream par_file((folder + "/" + par_filename_).c_str());
		if(!par_file.is_open()) 
			throw std::runtime_error("DataCreator GetController Cannot create parameter files.\n");
	
		for(unsigned int j = 0; j < par_list_[i].size(); ++j)
			par_file << par_name_[j] << " " << par_list_[i][j] << "\n";
		par_file.close();
	}

	// create parameter description and parameter prior
	std::ofstream parameter_description((EmulatorConstants::path + "DataCreator/parameter_description.dat").c_str());
	std::ofstream parameter_priors((EmulatorConstants::path + "DataCreator/parameter_priors.dat").c_str());
	if(! (parameter_description.is_open() && parameter_priors.is_open()))
		throw std::runtime_error("DataCreator GetController Cannot create parameter description or prior\n");
	for(unsigned int i = 0; i < par_upper_limit_.size(); ++i)
	{
		parameter_priors << "UNIFORM " << par_name_[i] << " " << par_lower_limit_[i] << " " << par_upper_limit_[i] << "\n";
		parameter_description << "UNIFORM " << par_name_[i] << " " << par_lower_limit_[i] << " " << par_upper_limit_[i] << "\n";
	}

	// first one is experimental data, need modifications later
	std::ofstream exp_file((EmulatorConstants::path + "template/experimental_output/DataCreator/results.dat").c_str());
	if(!exp_file.is_open()) 
		throw std::runtime_error("DataCreator GetController Cannot create Experimental files.\n");
	for(unsigned int j = 0; j < var_list_[0].size(); ++j)
		exp_file << var_name_[j] << " " << var_list_[0][j] << " " << var_error_list_[0][j] << "\n";
	exp_file.close();

	// observable names file
	std::ofstream obs_file((EmulatorConstants::path + "DataCreator/observable_names.dat").c_str());
	if(!obs_file.is_open()) 
		throw std::runtime_error("DataCreator GetController Cannot create observable names files.\n");
	for(unsigned int j = 0; j < var_name_.size(); ++j)
		obs_file << var_name_[j] << "\n";
	obs_file.close();

	return RunMaster(EmulatorConstants::path + "DataCreator", "DataCreator/");
}

