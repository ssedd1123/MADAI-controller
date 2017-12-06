#ifndef DATASELECTOR_H
#define DATASELECTOR_H
#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <regex>
#include <stdlib.h>
#include "EmulatorConstants.h"

class DataSelector
{
public:
	DataSelector(const std::string& t_dir, const std::string& t_template, int t_start, int t_end): 
		template_(t_template), start_no_(t_start), end_no_(t_end)
	{
		SetWorkingDir(t_dir);
		SingleOut(std::vector<int>{});
	};
        std::string GetDataDir(int);
	void SetWorkingDir(const std::string&);
	// rename the run files to single out a model run 
	void SingleOut(std::vector<int>);
	// set where to look for the template data
	void SetTemplate(const std::string& t_template){ template_ = t_template;};
        void SetExpResult(const std::string& t_filename = "original");
private:
	// flages to tell if there are runs that need 
	std::string path_;
	std::string original_path_;
	const std::string name_modified_ = "no_run"; 
	std::string template_;
	int start_no_, end_no_;
};

#endif
