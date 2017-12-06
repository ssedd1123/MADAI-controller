#include "TestModelExact/TestModelExact.h"

void TestModel(double t_S0 = 30, double t_L = 70, double ms = 0.8, double mv = 0.8)
{
	TestModelExact model;

	RunMaster emulator("/projects/hira/MADAI/Tsangc/TestModelEmulate/TestModelEmulate", "TestModelEmulate/");
	emulator.SetScales(0.9);
	emulator.SetNuggets(0.03548);

	std::vector<double> S0, L;
	// fix ms and mv for testing purposes

	std::vector<Data> data = model.Eval(t_S0, t_L, ms, mv);
	/*emulator.SetExpResult(data);

	emulator.GenerateTrace("", true, false);
	
	std::vector<double> val;
	emulator.GetTraceBest(val);
	std::cout << " Peak distribution : \n";
	std::cout << " S0 " << val[0] << " L " << val[1] << " ms " << val[2] << " mv " << val[3] << "\n\n";

	emulator.GetTraceBestLikelihood(val);
	std::cout << " Best likelihood : \n";
	std::cout << " S0 " << val[0] << " L " << val[1] << " ms " << val[2] << " mv " << val[3] << "\n";

	emulator.GetTraceMean(val);
	std::cout << " Mean distribution : \n";
	std::cout << " S0 " << val[0] << " L " << val[1] << " ms " << val[2] << " mv " << val[3] << "\n";
*/
	for(double val = 25.7; val < 36; val += 0.1) S0.push_back(val);
	for(double val = 30; val < 120; val += 1) L.push_back(val);

	//TH2F *hist = new TH2F("hist", "hist", S0.size()/2, (*S0.begin()), (*S0.end()), 46,29,119);//L.size()/2 + 1, (*L.begin()) - 1., (*L.end()) + 1);
	TGraph2D *graph = new TGraph2D;
	TGraph2D *graph_emulate = new TGraph2D;
	for(auto S0_it = S0.begin(); S0_it != S0.end(); ++S0_it)
		for(auto L_it = L.begin(); L_it != L.end(); ++L_it)
		{
			std::vector<Data> result = model.Eval(*S0_it, *L_it, ms, mv);
			std::vector<Data> result_emulate = emulator.RunEmulator(std::vector<double>{*S0_it, *L_it, ms, mv});
			// find chi-square of the result from data
			double chi_square = 0;
			double chi_square_emulate = 0;
			for(unsigned int i = 0; i < data.size(); ++i)
			{
				chi_square += pow((data[i].val - result[i].val), 2);
				chi_square_emulate += pow((data[i].val - result_emulate[i].val), 2);
			}

			graph->SetPoint(graph->GetN(), *S0_it, *L_it, chi_square);
			graph_emulate->SetPoint(graph_emulate->GetN(), *S0_it, *L_it, chi_square_emulate);
		}
	TCanvas *c1 = new TCanvas;
	c1->Divide(2,1);
	c1->cd(1);
	graph->Draw("colz");
	c1->cd(2);
	graph_emulate->Draw("colz");


	
}

