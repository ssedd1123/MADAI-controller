#include "DataSelector.h"

void DataSelector::SetWorkingDir(const std::string& t_path) 
{
	// path must not end in slash "/" 
	// check and see if we need to delete the slash
	std::regex path_format("(.*)\\/\\s*\\Z");
	std::smatch match;
	if(std::regex_match(t_path, match, path_format))
		original_path_ = match.str(1);
	else 
		original_path_ = t_path;
	path_ = original_path_ + "/model_output";

}

void DataSelector::SingleOut(std::vector<int> t_list)
{
	std::string command = "rm -rf " + path_ + "/run*";
	system(command.c_str()); 
	std::sort(t_list.begin(), t_list.end());
	unsigned int list_index = 0;
	int output_run_no = start_no_;
	for(int input_run_no = start_no_; input_run_no <= end_no_; ++input_run_no )
	{
		// if the current index is one of the indices given by t_list, we skip over it
		if(t_list.size() > 0)
			if(input_run_no == t_list[list_index])
			{
				// don't need to check unique vector for the last element
				if(list_index < t_list.size() - 1)
					if(t_list[list_index] == t_list[list_index + 1]) 
						throw std::runtime_error("SingleOut the input vector must be unique");
				++list_index;
				continue;
			}
		std::stringstream ss_output;
		// must be in format runxxxx
		ss_output << std::setw(4) << std::setfill('0') << output_run_no;
		command = "ln -s " + GetDataDir(input_run_no) + " " + path_ + "/run" + ss_output.str();
		//std::cout << "DataSelector " << command << std::endl;
		system(command.c_str());
		//std::cout << command << "\n";
		++output_run_no;
	}
}

std::string DataSelector::GetDataDir(int t_run)
{
	if(t_run > end_no_ || t_run < start_no_) throw std::runtime_error(" DataSelector GetDataDir index exceeded avaliable runs\n");
        std::stringstream ss_input;
	// must be in format run00xx
	ss_input << std::setw(4) << std::setfill('0') << t_run;
        return EmulatorConstants::path + "template/model_output/" + template_ + "run" + ss_input.str();
}
