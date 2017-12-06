#include "CSVReader.h"

void CSVReader::LoadFile(std::string t_filename)
{
	// clear everything before loading new files
	var_name_.clear();

	std::ifstream csv_file;
	csv_file.open(t_filename.c_str());
	if(!csv_file.is_open()) throw std::runtime_error("CSVReader LoadFile cannot open the required file");

	// regex rule for the first line (variables name)
	std::regex var_name_reg("\"(.*?)\",*+");
	// regex rule for variables 
	//std::regex var_val_reg("([-+]?[0-9]*\\.?[0-9]*),*+");
	std::smatch matches;

	// load variable names by reading the first line
	std::string line;
	if(std::getline(csv_file, line))
		while(std::regex_search(line, matches, var_name_reg))
		{
			var_name_.push_back(matches[1].str());
			line = matches.suffix();
		}
	else throw std::runtime_error("CSVReader LoadFile file is empty");
	/*
	// load variable values
	while(std::getline(csv_file, line))
	{
		int i = 0; // index of variables
		var line_var; // list of variables for each line
		while(std::regex_search(line, matches, var_val_reg))
		{
			line_var.insert(std::make_pair(var_name_[i], atof(matches[1].str().c_str())));
			line = matches.suffix();
			var_.push_back(line_var);
			std::cout << " Reading line " << i << "\r";
			++i;
			if(i >var_name_.size() - 1) break; // discard any data that is not named
		}
	}	
	std::cout << "\n";*/
	while(std::getline(csv_file, line))
		line_.push_back(line);
	csv_file.close();
}

double CSVReader::GetVarVal(unsigned int t_column, unsigned int t_row) // access with the row and column value
{
	if(t_row >line_.size() - 1) throw std::runtime_error("CSVReader GetVarVal row index larger than the file size");
	std::string numbers;
	std::stringstream ss(line_[t_row]);
	for(unsigned int i = 0; i < t_column ; ++i)
		if(!std::getline(ss, numbers, ',')) throw std::runtime_error("CSVReader GetVarVec cannot get value from required index");

	// load outside the loop, which is the number to be returned
	std::getline(ss, numbers, ',');
	return atof(numbers.c_str());
}

std::vector<double> CSVReader::GetVarVec(unsigned int t_start, unsigned int t_end, unsigned int t_row)
{
	if(t_end <= t_start) throw std::runtime_error("CSVReader GetVarVec start index must be smaller than end index");
	if(t_row >line_.size() - 1) throw std::runtime_error("CSVReader GetVarVec row index larger than the file size");
	std::vector<double> answer(t_end - t_start + 1);
	std::stringstream ss(line_[t_row]);
	for(unsigned int i = 0; i <= t_end; ++i)
	{
		std::string numbers;
		if(!std::getline(ss, numbers, ',')) throw std::runtime_error("CSVReader GetVarVec cannot get value from required index");
		if(i >= t_start) 
			answer[i - t_start] = atof(numbers.c_str());
	}
	return answer;
}
