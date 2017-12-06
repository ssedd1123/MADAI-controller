#ifndef DATACREATOR_H
#define DATACREATOR_H
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include "EmulatorConstants.h"
#include "RunMaster.h"

class DataCreator
{
public:
	// input are vectors of parameters name and variable names respectively
	DataCreator(const std::vector<std::string>&, const std::vector<std::string>&);

	void PushBack(const std::vector<double>&, const std::vector<double>&, const std::vector<double>&);
	void SetPoint(unsigned int, const std::vector<double>&, const std::vector<double>&, const std::vector<double>&);
	RunMaster GetController();	

private:
	void SetLimit(const std::vector<double>&);
	const std::string data_destination_ = EmulatorConstants::path + "template/model_output/DataCreator";
	const std::string prior_destination_ = EmulatorConstants::path + "template/priors/DataCreator";
	const std::string par_filename_ = "parameters.dat";
	const std::string var_filename_ = "results.dat";

	std::vector<std::string> par_name_;
	std::vector<std::string> var_name_;
	std::vector<double> par_upper_limit_;
	std::vector<double> par_lower_limit_;
	std::vector<std::vector<double>> par_list_;
	std::vector<std::vector<double>> var_list_;
	std::vector<std::vector<double>> var_error_list_;
	unsigned int par_num_;
	unsigned int var_num_;
	unsigned int event_num_;
	
};

#endif
