#include <random>

void FullTest()
{
	gStyle->SetPalette(kBird);

	/************************************************
	* This macro demonstrate how MADAI emulator work
	* by having it emulate a 1D function
	*************************************************/

	// choose the 1D function first
	TF2 *func = new TF2("func", "x*y*cos((y*y + x*x))", -M_PI/2., M_PI/2., -M_PI/2., M_PI/2.);

	// random number generator for noise generation
	const double noise = 0.05;
	std::default_random_engine generator(9);
	std::normal_distribution<double> distribution(-noise, noise);
	std::uniform_real_distribution<double> uniform(-M_PI/2., M_PI/2.);

	DataCreator creator(std::vector<std::string>{"x", "y"}, std::vector<std::string>{"z"});
	for(int i = 0; i < 100; ++i)
	{
		double x = uniform(generator);
		double y = uniform(generator);
		double z = func->Eval(x, y) + distribution(generator);
		creator.PushBack(std::vector<double>{x, y}, std::vector<double>{z}, std::vector<double>{noise});
	}

	RunMaster *controller = new RunMaster(creator.GetController());
	controller->SetSettings("EMULATOR_TRAINING_ALGORITHM", "exhaustive_geometric_kfold_common");
	controller->SetScales(0.269);//0.15);
	controller->SetNuggets(0.467);//1.8);

	TF2 *emu = new TF2("emu", [=](double *x, double *p){return controller->RunEmulator(vector<double>{x[0], x[1]})[0].val;}, -M_PI/2., M_PI/2., -M_PI/2., M_PI/2., 0);

	new TCanvas;
	func->Draw("colz");
	new TCanvas;
	emu->Draw("colz");
	//controller.SetSettings("EMULATOR_TRAINING_ALGORITHM", "exhaustive_geometric_kfold_common");
}
