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

#include "../src/TraceAnalyzer.cxx"
#include "../src/ExpResultSelector.cxx"
#include "../src/CSVReader.cxx"
#include "../src/DataSelector.cxx"
#include "../src/EmulatorInterface.cxx"
#include "../src/EmulatorOutputReader.cxx"
#include "../src/ProcessPipe.cxx"
#include "../src/RunInputReader.cxx"
#include "../src/SettingSetter.cxx"
#include "../src/RunMaster.cxx"

void DiffRi35(double S0= 25.7, double L = 30, double ms = 0.898, double mv = 0.8495)
{
	RunMaster model_emulate("/projects/hira/MADAI/Tsangc/Ri35_Emulate/Ri35_simple", "Ri35_simple/");
	RunMaster model_exact("/projects/hira/MADAI/Tsangc/Ri35_2/RiE35", "Ri35/");
	// configurations to be looped through
	std::vector<double> nuggets{/*0.001*/pow(10, -0.42)};
	std::vector<double> scales{/*0.01*/1};

	model_emulate.SetScales(1.);
	model_emulate.SetNuggets(pow(10, -0.42));

	model_exact.SetScales(1.);
	model_exact.SetNuggets(pow(10, -0.42));
	
	std::cout << " Enter S0, L, ms, mv\n";
	while(std::cin >> S0 >> L >> ms >> mv)
	{
	
		std::cout << S0 << " " << L << " " << ms << " " << mv << "\n";
		TGraphErrors *graph_exact = new TGraphErrors;
		TGraphErrors *graph_emulate = new TGraphErrors;
	
		vector<Data> data_emulate = model_emulate.RunEmulator(std::vector<double>{S0, L, ms, mv});
		vector<Data> data_exact = model_exact.RunEmulator(std::vector<double>{S0, L, ms, mv});
	
		for(int index = 0; index < data_emulate.size(); ++index)
		{	
			graph_exact->SetPoint(index, (double) index, data_exact[index].val);
			graph_exact->SetPointError(index, 0, data_exact[index].err);
	
			graph_emulate->SetPoint(index, (double) (index) + 0.2, data_emulate[index].val);
			graph_emulate->SetPointError(index, 0, data_emulate[index].err);
		}

		TMultiGraph *mg = new TMultiGraph;
		mg->Add(graph_exact);
		mg->Add(graph_emulate);
		TCanvas *c1 = (TCanvas*) gROOT->FindObject("c1");
		if(!c1) c1 =new TCanvas("c1");
		mg->Draw("AP");
		gPad->WaitPrimitive();
	}
}
