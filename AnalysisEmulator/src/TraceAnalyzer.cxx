#include "TraceAnalyzer.h"

void TraceAnalyzer::AnalyzeTrace(const std::string& t_csv, const std::string& t_path, unsigned int t_downscale)
{
	// clear anything from previous analysis
	mean_.clear(); std_dev_.clear(); scaled_dev_.clear(); best_.clear();
	name_.clear();

	// read the parameter prior files to get the range and name of each variables
	std::ifstream prior(t_path + "/parameter_priors.dat");
	if(!prior.is_open())
		throw std::runtime_error("TraceAnalyzer AnalyzeTrace cannot find parameter priors.");
	
	std::string line;
	std::vector<std::string> para_name;
	std::vector<double> para_min, para_max;
	while(std::getline(prior, line))
	{
		std::string temp1, temp2;
		double min, max;
		std::stringstream ss(line);
		if(ss >> temp1 >> temp2 >> min >> max)
		{
			para_name.push_back(temp2);
			para_min.push_back(min);
			para_max.push_back(max);
		}else throw std::runtime_error("TraceAnalyzer AnalyzeTrace parameter prior is not in the right format");
		
	}

	// TH1D for calculation of peaks, mean and sd of the data
	std::map<std::string, TH1D> hist;
	for(unsigned int i = 0; i < para_name.size(); ++i)
		hist.insert({para_name[i], TH1D(para_name[i].c_str(), "", 100, para_min[i], para_max[i])});

	// read the csv file
	std::ifstream csv_file(t_csv);
	if(!csv_file.is_open()) throw std::runtime_error("TraceAnalyzer AnalyzeTrace csv file is not found");
	// read the first line for parameters ordering
	if(std::getline(csv_file, line))
	{
		std::string temp;
		std::stringstream ss(line);
		for(unsigned int i = 0; i < para_name.size(); ++i)
		{
			std::getline(ss, temp, ',');
			// remove the first and last character as they are (")s
			if(temp.size() > 1)
				temp = temp.substr(1, temp.size() - 2);
			else throw std::runtime_error("TraceAnalyzer AnalyzeTrace names from csv file not valid");
			name_.push_back(temp);
		}

	}

	// find the one with largest log likelihood
	double likelihood = - std::numeric_limits<double>::infinity();
	// fill the th1d
	// starts from -1 becaues it will be incremented immediately inside the loop
	unsigned int no_line = -1;
	while(std::getline(csv_file, line))
	{
		++no_line;
		if(no_line % t_downscale != 0) continue;

		std::stringstream ss(line);
		std::string temp;
		std::vector<double> par_temp(name_.size());
		for(unsigned int i = 0; i < name_.size(); ++i)
		{
			std::getline(ss, temp, ',');
			double val = atof(temp.c_str());
			hist.at(name_[i]).Fill(val);
			par_temp[i] = val;
		}
		// last colume is the log likelihood
		while(std::getline(ss, temp, ',')){}

		if(atof(temp.c_str()) > likelihood) 
		{
			likelihood = atof(temp.c_str());
			best_likelihood_ = par_temp;
		}
	}


	// get statistical values from th1d
	double mean, std_dev, scaled_dev, best;
	for(unsigned int i = 0; i < name_.size(); ++i)
	{
		mean_.push_back(hist.at(name_[i]).GetMean());
		std_dev_.push_back(hist.at(name_[i]).GetStdDev());
		best_.push_back(hist.at(name_[i]).GetBinCenter(hist.at(name_[i]).GetMaximumBin()));
	}
	
}

