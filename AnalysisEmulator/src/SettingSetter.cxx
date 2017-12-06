#include "SettingSetter.h"

SettingSetter::SettingSetter(const std::string& t_input_template_file, const std::string& t_output_path) : 
	input_template_file_(t_input_template_file), 
	output_path_(t_output_path),
	applied_changes_(false)
	{Reset();}

void SettingSetter::Set(const std::string& t_setting_name, const std::string& t_setting_var)
{
	// check if the channging field has previously changed
	//if(!_settings.insert(std::pair<std::string, std::string>(t_setting_name, t_setting_var)).second)
		settings_[t_setting_name] = t_setting_var;
		applied_changes_ = false;	
}

void SettingSetter::GetListOfChanges(	std::vector<std::string>& t_setting_name, 
					std::vector<std::string>& t_setting_var)
{
	t_setting_name.clear();
	t_setting_var.clear();
	for(auto it = settings_.begin(); it != settings_.end(); ++it)
	{
		t_setting_name.push_back(it->first);
		t_setting_var.push_back(it->second);
	}
}

void SettingSetter::ApplySetting(bool t_reset)
{
	std:;string command;
	if(t_reset) Reset();
	for(auto it = settings_.begin(); it != settings_.end(); ++it)
	{
		command = 	"madai_change_setting " + 
				output_path_ + " " +
				it->first + " " + 
				it->second + " > /dev/null";
		//cout << command << endl;
		//ProcessPipe newpipe(command);
		//pipe.GetOutput();
		system(command.c_str());
	}
	if(t_reset) settings_.clear();
	applied_changes_ = true;
}

void SettingSetter::Reset() 
{ 
	applied_changes_ = false; 
	// copy template settings to destination before modifying it
	std::string command = "/bin/cp " + input_template_file_ + " " + output_path_ + "/settings.dat";
	//ProcessPipe oldpipe(command);
	system(command.c_str());
};
