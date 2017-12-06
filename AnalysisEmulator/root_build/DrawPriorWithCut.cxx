void DataToGraph(const std::vector<Data> t_data, TGraph* graph)
{
	for(unsigned int i = 0; i < t_data.size(); ++i)
		graph->SetPoint(graph->GetN(), (double) i, t_data[i].val);
}

void DrawPriorWithCut(	double min_S0 = 25.7, double max_S0 = 36,
			double min_L = 32, double max_L = 120, 
			double min_ms = 0.6, double max_ms = 1.,
			double min_fi = -0.59, double max_fi = 0.66,
			EColor t_color = kBlue, TCanvas *c1 = 0)
{
	gStyle->SetPalette(kBird);

	std::string emulator_file = "/mnt/analysis/hira/tsangc/new_distribution/e120";
	std::string emulator_mode = "e120_sn112_spectra/";

	RunMaster controller(emulator_file, emulator_mode);
	controller.SetScales(0.625);
	controller.SetNuggets(1.001);
	std::vector<Data> exp_result = controller.GetExpResult();

	// draw prior
	bool canvas_provided = true;
	if(!c1) 
	{
		c1 = new TCanvas;
		canvas_provided = false;
	}
	std::vector<TGraph*> emulator_priors;
	for(unsigned int i = 0; i < 1000; i += 1)
	{
		double nS0 = (rand() / (float)RAND_MAX*(max_S0 - min_S0)) + min_S0;
		double nL = (rand() / (float)RAND_MAX*(max_L - min_L)) + min_L;
		double nms = (rand() / (float)RAND_MAX*(max_ms - min_ms)) + min_ms;
		double nfi = (rand() / (float)RAND_MAX*(max_fi - min_fi)) + min_fi;
		std::vector<Data> result = controller.RunEmulator(std::vector<double>{nS0, nL, nms, nfi});
		emulator_priors.push_back(new TGraph);
		DataToGraph(result, emulator_priors.back());
		emulator_priors.back()->SetLineColor(t_color);
		emulator_priors.back()->Draw((i == 0 && !canvas_provided) ? "AL" :"Lsame");
		emulator_priors.back()->GetYaxis()->SetRangeUser(0.1, 5.5);
	}

	TGraphErrors *exp_graph = new TGraphErrors;
	DataToGraph(exp_result, exp_graph);
	for(unsigned int i = 0; i < exp_result.size(); ++i)
		exp_graph->SetPointError(i, 0, exp_result[i].err);

	exp_graph->SetMarkerStyle(8);
	exp_graph->Draw("PLsame");
}
