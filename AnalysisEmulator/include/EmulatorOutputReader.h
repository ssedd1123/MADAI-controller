#ifndef EMULATOROUTPUTREADER_H
#define EMULATOROUTPUTREADER_H
#include <string>
#include <regex>
#include <map>
#include <fstream>
#include <stdexcept>
#include "EmulatorConstants.h"

typedef std::vector<double> row;
typedef std::vector<row> matrix;

class EmulatorOutputReader
{
public:
	EmulatorOutputReader() : var_num_(0) {};
	~EmulatorOutputReader(){};
	void LoadFile(const std::string&);
	void GetVarName(std::vector<std::string>&);
	int GetVarIndex(const std::string&);
	double GetVar(unsigned int);
	double GetCovMatrix(unsigned int, unsigned int);
	int GetVarNum();
private:
	// needed for efficienct variable name search
	std::map<std::string, int>	var_to_index_;
	std::vector<std::string>	var_name_;
	row 	var_value_;
	matrix	cov_value_;
	unsigned int	var_num_;
};

#endif
