#ifndef SETTINGSETTER_H
#define SETTINGSETTER_H
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "ProcessPipe.h"

class SettingSetter
{
public:
	SettingSetter(const std::string&, const std::string&);
	~SettingSetter(){};
	void Reset();// { settings_.clear(); applied_changes_ = false; };
	void Set(const std::string&, const std::string&);
	void GetListOfChanges(	std::vector<std::string>&, 
				std::vector<std::string>&);
	bool ChangesApplied() { return applied_changes_; };
	void ApplySetting(bool t_reset = true);
private:
	// flag to see if all changes have been applied
	bool applied_changes_;
	std::string output_path_;
	std::string input_template_file_;
	// vectors storing changes made to the default setting files
	// copy setting files only when ApplySetting is used
	// that way we can be sure that No change other than specified by this setter is applied
	std::map<std::string, std::string>  settings_;
};

#endif
