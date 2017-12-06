#include "EmulatorOutputReader.h"

void EmulatorOutputReader::LoadFile(const std::string& t_filename)
{
	// reset everything when a new file is being loaded
	var_name_.clear();
	var_to_index_.clear();
	
	// load file
	std::ifstream emulator_output(t_filename.c_str());
	if(!emulator_output.is_open())
		throw std::runtime_error("LoadFile File failed to opened");
	std::string line;
	while(std::getline(emulator_output, line))
	{
		if(line == "OUTPUTS")
		{
			// next line contains the number of variables
			std::getline(emulator_output, line);
			var_num_ = atoi(line.c_str());
			while(std::getline(emulator_output, line))
			{
				if(line == "COVARIANCE") break;
				var_name_.push_back(line);
				var_to_index_.insert(std::make_pair(line, var_name_.size() - 1));
			}
			if(var_num_ != var_name_.size())
				throw std::runtime_error("LoadFile Variables size not consistance");
		}else if(line == "END_OF_HEADER")
		{
			// read mean value and matrix
			var_value_.resize(var_num_);
			cov_value_.resize(var_num_);
			for(unsigned int i = 0; i < var_num_; ++i)
			{
				std::getline(emulator_output, line);
				var_value_[i] = atof(line.c_str());
			} 	
			// read the covariance matrix
			for(unsigned int i = 0; i < var_num_; ++i)
			{
				cov_value_[i].resize(var_num_);
				for(unsigned int j = i; j < var_num_; ++j)
				{
					std::getline(emulator_output, line);
					cov_value_[i][j] = atof(line.c_str());
				}
			}
			// this should be the end of file
			// something is wrong if it isn't 
			std::getline(emulator_output, line);
			if(!emulator_output.eof()) throw std::runtime_error("LoadFile file not reaching the end after covariance matrix");
		}
	}
	emulator_output.close();
}

int EmulatorOutputReader::GetVarNum()
{
	if(var_num_ == 0) throw std::runtime_error("GetVarNum 0 variables returned, output files not read correctly");
	return var_num_;
}

void EmulatorOutputReader::GetVarName(std::vector<std::string>& t_input)
{
	t_input.resize(var_num_);
	for(unsigned int i = 0; i < var_num_; ++i) t_input[i] = var_name_[i];
}

int EmulatorOutputReader::GetVarIndex(const std::string& t_input)
{
	std::map<std::string, int>::iterator it = var_to_index_.find(t_input.c_str());
	if(it == var_to_index_.end()) throw std::runtime_error("GetVarIndex variable name not found");
	return it->second;
}

double EmulatorOutputReader::GetVar(unsigned int t_i)
{
	if(t_i > var_value_.size()) throw std::runtime_error("GetVar index out of range");
	return var_value_[t_i];
}

double EmulatorOutputReader::GetCovMatrix(unsigned int t_i, unsigned int t_j)
{
	// swap the indices if t_i > t_j
	if(t_i > t_j) 
	{
		int temp;
		temp = t_j;
		t_j = t_i;
		t_i = temp;
	}
	// it is triangular matrix, it should be fine
	if(t_i > cov_value_.size()) throw std::runtime_error("GetCovMatrix index out of range");
	return cov_value_[t_i][t_j];
}
