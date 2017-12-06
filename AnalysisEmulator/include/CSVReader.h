#ifndef CSVREADER_H
#define CSVREADER_H
#include <regex>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>

class CSVReader
{
public:
	CSVReader(){};
	~CSVReader(){};
	void LoadFile(std::string);
	// return vector containning lists of variable names
	std::vector<std::string> GetVarName() { return var_name_; };
	// all variables indices start from 0
	double GetVarVal(unsigned int, unsigned int); // access with the row and column value
	std::vector<double> GetVarVec(unsigned int, unsigned int, unsigned int);	// get a bunch of variables as vector at once
	int GetVarNum() { return var_name_[0].size(); };
	int GetDataNum() { return var_name_.size(); };
private:
	std::vector<std::string> var_name_;
	std::vector<std::string> line_;
};


#endif
