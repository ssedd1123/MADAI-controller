#ifndef EXPRESULTSELECTOR_H
#define EXPRESULTSELECTOR_H
#include "EmulatorConstants.h"
#include <string>
#include <regex>
#include <sstream>

class ExpResultSelector
{
public:
	ExpResultSelector(const std::string& t_dir, const std::string& t_template) : template_(t_template)
	{
		SetWorkingDir(t_dir);
		SetExpResult();
	};
	~ExpResultSelector(){};
	void SetWorkingDir(const std::string&);
        void SetTemplate(const std::string& t_template){ template_ = t_template;};
	void SetExpResult(const std::string& t_filename = "original");
	std::string GetExpPath();
private:
	std::string path_;
	std::string template_;
	std::string file_path_;
};

#endif
