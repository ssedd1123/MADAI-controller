#ifndef TESTMODELEXACT_H
#define TESTMODELEXACT_H
#include "ProcessPipe.h"
#include "test.h"
#include "TreeFiller.h"
#include "EmulatorInterface.h"
#include "DataSelector.h"
#include "RunInputReader.h"
#include "RunMaster.h"
#include "EmulatorConstants.h"
#include "CSVReader.h"
#include "SettingSetter.h"
#include "Data.h"
#include "TFile.h"
#include "TH2.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>
#include <regex>
#include <iomanip>
#include <regex>

class TestModelExact
{
public:
	TestModelExact() : controller("/projects/hira/MADAI/Tsangc/TestModel/TestModel", "TestModelExact/")
	{
		controller.SetScales(1);
		controller.SetNuggets(pow(10, -0.42));
	}

	std::vector<Data> Eval(double S0, double L, double ms, double mv)
	{ return controller.RunEmulator(std::vector<double>{S0, L, ms, mv});};
	void GetVarName(std::vector<std::string>& t_string){ controller.GetVarName(t_string);};
	void GetParName(std::vector<std::string>& t_string){ controller.GetParName(t_string);};
private:
	RunMaster controller;

};

#endif
