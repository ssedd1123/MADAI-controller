#include <random>

void DemonstrationEmulator()
{
	/************************************************
	* This macro demonstrate how MADAI emulator work
	* by having it emulate a 1D function
	*************************************************/

	// choose the 1D function first
	TF1 *func = new TF1("func", "x*sin(x)", 0, 10);

	// random number generator for noise generation
	const double noise = 0.5;
	std::default_random_engine generator(9);
	std::normal_distribution<double> distribution(0, noise);
	std::uniform_real_distribution<double> unif(0, 11);

	// generate and record sample points
	TGraphErrors *graph = new TGraphErrors;
	DataCreator creator(std::vector<std::string>{"x"}, std::vector<std::string>{"y"});
	for(double x = 0; x <= 11; x += 0.5)
	{
		double x_new = x;//unif(generator);
		double y = func->Eval(x_new) + distribution(generator);
		//x_new *= 100;
		creator.PushBack(std::vector<double>{x_new}, std::vector<double>{y}, std::vector<double>{noise});
		int num = graph->GetN();
		graph->SetPoint(num, x_new, y);
		graph->SetPointError(num, 0, noise);
	}

	RunMaster controller = creator.GetController();
	//controller.SetSettings("EMULATOR_TRAINING_ALGORITHM", "exhaustive_geometric_kfold_common");
	controller.SetScales(0.01);///*0.3*/0.23148);
	controller.SetNuggets(0.19);////0.132);

	// store upper and lower band line into vectors to draw error band
	std::vector<double> xval, upper, lower;
	TGraph *emulator = new TGraph;
	TGraph *emulator_error = new TGraph;
	TGraph *just_error = new TGraph;
	TGraph *expected_improvement = new TGraph;

	double look_for_point = -0.9;

	for(double x = 0.04; x <= 10; x += 0.001)
	{
		xval.push_back(x);
		Data data = controller.RunEmulator(std::vector<double>{x})[0];
		emulator->SetPoint(emulator->GetN(), x, data.val);
		upper.push_back(data.val + 1.96*data.err);
		lower.push_back(data.val - 1.96*data.err);
		just_error->SetPoint(just_error->GetN(), x, data.err);
		//expected_improvement->SetPoint(expected_improvement->GetN(), x, exp(
	}

	// fill the error band
	for(unsigned int i = 0; i < upper.size(); ++i)
		emulator_error->SetPoint(emulator_error->GetN(), xval[i], upper[i]);

	unsigned int lower_size = lower.size();
	for(unsigned int i = 0; i < lower_size; ++i)
		emulator_error->SetPoint(emulator_error->GetN(), xval[lower_size - 1 - i], lower[lower_size - 1 - i]);

	// final point must equal to initial point to complete a shape
	double x, y;
	emulator_error->GetPoint(0, x, y);
	emulator_error->SetPoint(emulator_error->GetN(), x, y);

	// take care of the formate of graphs and draw them
	graph->SetMarkerStyle(8);
	func->SetLineColor(kBlue);
	func->SetLineStyle(7);
	emulator->SetLineColor(kRed);
	emulator_error->SetFillStyle(3001);
	emulator_error->SetFillColor(kGreen);
	new TCanvas;
	TMultiGraph *mg = new TMultiGraph;
	emulator_error->Draw("af");
	emulator_error->Draw("lsame");
	emulator->Draw("lsame");
	func->Draw("same");
	func->GetXaxis()->SetRangeUser(0,10);
	func->GetXaxis()->SetTitle("x");
	func->GetYaxis()->SetTitle("y");
	graph->Draw("Psame");
	//mg->Add(emulator_error);
	//mg->Add(graph);
	//mg->Add(emulator);	
	//mg->Draw();


	auto legend = new TLegend(0.1,0.7,0.3,0.9);
	legend->AddEntry(func, "f(x) = x*sin(x)", "l");
	legend->AddEntry(graph, "Training points", "p");
	legend->AddEntry(emulator, "Gaussian Emulator", "l");
	legend->AddEntry(emulator_error, "95\% confidence interval", "F");
	legend->Draw();

	new TCanvas;
	just_error->Draw("AL");
}
