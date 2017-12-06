void CSVToRoot(const std::string& t_filename = "../e120.csv", const std::string& t_output = "e120.root")
{
	// number of variables
	const int var_no = 4;

	std::vector<std::string> var_name_list(var_no);
	std::vector<double> var_val_list(var_no);

	// output root file
	TFile *file = new TFile(t_output.c_str(), "RECREATE");
	TTree *tree = new TTree;

	// read the csv file
	std::ifstream csv_file(t_filename);
	if(!csv_file.is_open())
	{
		std::cerr << "File not opened!\n";
		return;
	}

	// look for names of the variables in the first line of the csv file
	std::string line;
	if(std::getline(csv_file, line))
	{
		// parse it with regex to get names inside the bracket ""
		std::regex rule("\\\"(.*?)\\\"");
		std::smatch base_match;
		for(int counter = 0; counter < var_no; ++counter)
			if(std::regex_search(line, base_match, rule))
			{
				var_name_list[counter] = base_match[0];
				var_name_list[counter].erase(0,1);
				var_name_list[counter].erase(var_name_list[counter].size() - 1);
				line = base_match.suffix();
			}else
			{
				std::cerr << " Cannot read variables name from csv file!\n";
				return;
			}
	}else
	{
		std::cerr << " CSV file is empty!\n";
		return;
	}

	// set up trees to load the csv file
	for(int counter = 0; counter < var_no; ++counter)
		tree->Branch(var_name_list[counter].c_str(), &var_val_list[counter]);

	// read each line, each data is delimited by a comma ,
	while(std::getline(csv_file, line))
	{
		// use stringstream to delimite data with comma
		std::stringstream ss(line);
		for(int counter = 0; counter < var_no; ++counter)
		{
			std::string value;
			if(std::getline(ss, value, ','))
				var_val_list[counter] = atof(value.c_str());
			else
			{
				std::cerr << " Cannot read values from csv file!\n";
				return;
			}
		}
		tree->Fill();
	}	

	tree->Write("tree");
	file->Close();
	delete tree;
	delete file;
}

void DataToGraph(const std::vector<Data> t_data, TGraph* graph)
{
	for(unsigned int i = 0; i < t_data.size(); ++i)
		graph->SetPoint(graph->GetN(), (double) i, t_data[i].val);
}

TCanvas* EmulatorAfterCut(TCutG *t_cutg = 0, TCanvas *c2 = 0, int t_color = kRed)
{
	gStyle->SetPalette(kBird);

	std::string emulator_file = "/mnt/analysis/hira/tsangc/new_distribution/e120";
	std::string emulator_mode = "e120_sn112_spectra/";

	std::string csv_file = "/mnt/analysis/hira/tsangc/new_distribution/e120.csv";
	std::string root_file = "ROOT/cut_file.root";

	CSVToRoot(csv_file, root_file);

	// open root file such that cuts on S0 and L can be shown/drawn
	TFile *file = new TFile(root_file.c_str());
	if(!file->IsOpen())
		throw std::runtime_error("File cannot be opened\n");

	TTree *tree = (TTree*) file->Get("tree");
	if(!tree)
		throw std::runtime_error("Tree not found in the file");

	TCanvas *c1 = new TCanvas;

	if(!t_cutg)
	{
		delete gROOT->FindObject("CUTG");
		tree->Draw("S0:L", "", "colz");
		gPad->WaitPrimitive();
		t_cutg = (TCutG*) gROOT->GetListOfSpecials()->FindObject("CUTG");	
	}

	std::string cut_name = "cutg";
	delete gROOT->FindObject(cut_name.c_str());
	t_cutg->SetName(cut_name.c_str());
	t_cutg->SetVarX("L");
	t_cutg->SetVarY("S0");
	tree->Draw("S0:L", cut_name.c_str(), "colz");

	RunMaster controller(emulator_file, emulator_mode);
	controller.SetScales(0.625);
	controller.SetNuggets(1.001);
	std::vector<Data> exp_result = controller.GetExpResult();

	// draw experiment results 
	bool provided_canvas =true;
	if(!c2) 
	{
		c2 = new TCanvas;
		provided_canvas = false;
	}
	TGraphErrors *exp_graph = new TGraphErrors;
	DataToGraph(exp_result, exp_graph);
	for(unsigned int i = 0; i < exp_result.size(); ++i)
		exp_graph->SetPointError(i, 0, exp_result[i].err);

	exp_graph->SetMarkerStyle(8);
	if(!provided_canvas) 
	{
		std::cout << " Draw Again\n";
		exp_graph->Draw("APL");
	}

	tree->Draw(">>elist", cut_name.c_str());
	TEventList *elist = (TEventList*)gDirectory->Get("elist");

	unsigned int num_entries = elist->GetN();

	// draw prior
	std::vector<TGraph*> emulator_priors;
	for(unsigned int i = 0; i < 10000; i += 1)
	{
		double nS0 = (rand() / (float)RAND_MAX*(36 - 25.7)) + 25.7;
		double nL = (rand() / (float)RAND_MAX*(120 - 32)) + 32;
		double nms = (rand() / (float)RAND_MAX*(1 - 0.6)) + 0.6;
		double nfi = (rand() / (float)RAND_MAX*(0.66 + 0.59)) - 0.59;
		std::vector<Data> result = controller.RunEmulator(std::vector<double>{nS0, nL, nms, nfi});
		emulator_priors.push_back(new TGraph);
		DataToGraph(result, emulator_priors.back());
		c2->cd();
		emulator_priors.back()->SetLineColor(kBlue);
		emulator_priors.back()->Draw("Lsame");
	}

	// load variables from the tree
	double S0, L, ms, fi;
	tree->SetBranchAddress("S0", &S0);
	tree->SetBranchAddress("L", &L);
	tree->SetBranchAddress("ms", &ms);
	tree->SetBranchAddress("fi", &fi);

	std::vector<TGraph*> emulator_graphs;
	for(unsigned int i = 0; i < num_entries / 1000; i += 1)
	{
		tree->GetEntry(elist->GetEntry(i));
		std::vector<Data> result = controller.RunEmulator(std::vector<double>{S0, L, ms, fi});
		emulator_graphs.push_back(new TGraph);
		DataToGraph(result, emulator_graphs.back());
		c2->cd();
		emulator_graphs.back()->SetLineColor(t_color);
		emulator_graphs.back()->Draw("Lsame");
	}

	exp_graph->Draw("PLsame");

	return c2;
}

void EmulatorAfterCutBoth()
{
	TCanvas *c1 = EmulatorAfterCut();
	EmulatorAfterCut(0, c1, kGreen);
	EmulatorAfterCut(0, c1, kBlue);
}
