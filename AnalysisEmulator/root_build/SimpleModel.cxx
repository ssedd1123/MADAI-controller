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
#include <random>

#include "../src/ExpResultSelector.cxx"
#include "../src/CSVReader.cxx"
#include "../src/DataSelector.cxx"
#include "../src/EmulatorInterface.cxx"
#include "../src/EmulatorOutputReader.cxx"
#include "../src/ProcessPipe.cxx"
#include "../src/RunInputReader.cxx"
#include "../src/SettingSetter.cxx"
#include "../src/RunMaster.cxx"

std::vector<int> RandomNonRepeat(int t_min, int t_max)
{
	std::vector<int> numbers;
	for(int i = t_min; i <= t_max; ++i) numbers.push_back(i);
	std::random_shuffle(numbers.begin(), numbers.end());
	return numbers;
}

void SimpleModel(int no_included = 3)
{
	RunMaster controller("/projects/hira/MADAI/Tsangc/simple_model/simple_model", "simple_model/");

	// exclude different runs for a few times
	for(int counter = 0; counter < 5; ++counter)
	{
		std::vector<int> Excluded(RandomNonRepeat(controller.GetStartNo(), controller.GetEndNo()));
		controller.ExcludeRun(std::vector<int>(Excluded.begin() + no_included, Excluded.end()));
		std::string label("Trained_with_run");
		for(unsigned int i = 0; i < no_included; ++i)
		{
			cout << Excluded[i] << " " << endl;
			label += "_" + std::to_string(Excluded[i]);
		}
		controller.SetScales(0.785);
		controller.SetNuggets(pow(10, -3.43));
		controller.GenerateTrace(label);
	}
}
