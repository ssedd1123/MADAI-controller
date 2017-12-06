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
#include "TestModelExact/TestModelExact.h"

void RunFromConfig()
{
	TestModelExact model;

	// read from configuration files
	std::ifstream file("ConfigFile.dat");
	std::string line;

	while(std::getline(file, line))
	{
		std::stringstream ss, foldername;
		ss << line;
		int run_no;
		double S0, L, ms, mv;
		ss >> run_no >> S0 >> L >> ms >> mv;
		
		// file name to which results are written to
		foldername << "model_output/run" << setw(4) << setfill('0') << run_no;
		std::ofstream results(foldername.str() + "/results.dat");
		std::ofstream parameters(foldername.str() + "/parameters.dat");

		// load variable and parameter names
		std::vector<std::string> var_name, par_name;
		model.GetVarName(var_name);
		model.GetParName(par_name);

		// run emulator
		std::vector<Data> data = model.Eval(S0, L, ms, mv);
		
		// write data into the correct file
		parameters << "S0 " << S0 << "\nL " << L << "\nms " << ms << "\nmv " << mv << std::endl;

		for(int i = 0; i < data.size(); ++i)
		{
			results << var_name[i] << " " << data[i].val << " " << data[i].err << "\n";
		}
		
	}
}
