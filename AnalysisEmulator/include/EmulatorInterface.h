#ifndef EMULATORINTERFACE_H
#define EMULATORINTERFACE_H
#include <string>
#include <stdlib.h>
#include <regex>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <fstream>
#include "EmulatorConstants.h"
#include "ProcessPipe.h"

class EmulatorInterface
{
public:
	EmulatorInterface(const std::string& t_dir, int t_num): 
		expected_par_num_(t_num), pipe_(0)
	{
		SetWorkingDir(t_dir); 
	};
	~EmulatorInterface();
	void TrainEmulator(bool t_pca = true);
	void RunEmulator(const std::vector<double>&);
	void SetWorkingDir(std::string);
	int GetVarIndex(const std::string&);
	double GetVar(int);
	double GetCovMatrix(int, int);
	int GetVarNum();
	void GetVarName(std::vector<std::string>&);
private:
	//EmulatorOutputReader reader_;
	ProcessPipe *pipe_; // must be a pointer in order to delay its construction in setwokringdir
	std::string path_;
	int expected_par_num_;
	int var_num_;
	// vectors storing values of each variables
	std::vector<double> var_;
	// covariance matrix
	std::vector<std::vector<double>> cov_;
};

#endif
