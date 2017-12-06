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

/*#include "../src/TraceAnalyzer.cxx"
#include "../src/ExpResultSelector.cxx"
#include "../src/CSVReader.cxx"
#include "../src/DataSelector.cxx"
#include "../src/EmulatorInterface.cxx"
#include "../src/EmulatorOutputReader.cxx"
#include "../src/ProcessPipe.cxx"
#include "../src/RunInputReader.cxx"
#include "../src/SettingSetter.cxx"
#include "../src/RunMaster.cxx"
*/
void VisualizeData()
{
	TFile *file = new TFile("VisualizeDataRi35.root", "RECREATE");
	TTree *tree = new TTree;

	double S0, L, ms = 0.898, mv = 0.8495;
	double val, err;
	int index;
	tree->Branch("S0", &S0);
	tree->Branch("L", &L);
	tree->Branch("ms", &ms);
	tree->Branch("mv", &mv);
	tree->Branch("val", &val);
	tree->Branch("err", &err);
	tree->Branch("index", &index);

	RunMaster controller("/projects/hira/MADAI/Tsangc/Ri35_2/RiE35", "Ri35/");
	controller.SetScales(1.);
	controller.SetNuggets(pow(10, -0.42));
	for(ms = 0.6; ms < 1.; ms +=0.02)
		for(mv = 0.6; mv < 1.1; mv+=0.02)
			for(S0 = 25.7; S0 < 36; S0 += 0.2)
				for(L = 30; L < 120; L +=2)
				{

					std::cout << "\r S0 = " << S0 << " L = " << L << " ms = " << ms << " mv = " << mv ;
					std::vector<Data> data = controller.RunEmulator(std::vector<double>{S0, L, ms, mv});
					for(index = 0; index < data.size(); ++index)
					{
						val = data[index].val;
						err = data[index].err;
						tree->Fill();
					}
				}
	tree->Write("ttree");
}
