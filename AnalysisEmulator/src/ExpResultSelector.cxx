#include "ExpResultSelector.h"

void ExpResultSelector::SetWorkingDir(const std::string& t_path)
{
        // path must not end in slash "/"
        // check and see if we need to delete the slash
        std::regex path_format("(.*)\\/\\s*\\Z");
        std::smatch match;
        if(std::regex_match(t_path, match, path_format))
                path_ = match.str(1);
        else
                path_ = t_path;
        path_ = path_ + "/experimental_data";

}

void ExpResultSelector::SetExpResult(const std::string& t_filename)
{
	std::string command = "rm -rf " + path_ + "/results.dat";
	system(command.c_str());
	if(t_filename == "original") 
		file_path_ = EmulatorConstants::path + "/template/experimental_output/" + template_ + "results.dat";
	else  
		file_path_ = t_filename;
	command = "ln -s " + file_path_ + " " + path_ + "/results.dat";
        system(command.c_str());
}

std::string ExpResultSelector::GetExpPath()
{
	if(file_path_.empty())
		throw std::runtime_error("ExpResultSelector GetExpPath you need to set exp result before getting the path\n");
	return file_path_;	
}
