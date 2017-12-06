#ifndef RUNINPUTREADER_H
#define RUNINPUTREADER_H
#include <iostream>
#include <cmath>
#include <string>
#include <regex>
#include <map>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include "EmulatorConstants.h"

class RunInputReader
{
public:
	RunInputReader(const std::string& t_template) : template_(t_template), var_num_(0), par_num_(0) {};
	~RunInputReader(){};
	void LoadFile(int);
	void LoadRunFile(const std::string&);
	void LoadParFile(const std::string&);
	void GetVarName(std::vector<std::string>&);
	int GetVarIndex(const std::string&);
	double GetVar(unsigned int);
	double GetError(unsigned int);
	void GetParName(std::vector<std::string>&);
	int GetParIndex(const std::string&);
	double GetPar(unsigned int);
	int GetVarNum();
	int GetParNum();
private:
	std::string template_;
	std::map<std::string, int>      var_to_index_;
        std::vector<std::string>        var_name_;
        std::vector<double>     var_value_;
	std::vector<double>	var_error_;
        int     var_num_;

	std::map<std::string, int>      par_to_index_;
        std::vector<std::string>        par_name_;
        std::vector<double>     par_value_;
	int	par_num_;
};

#endif

