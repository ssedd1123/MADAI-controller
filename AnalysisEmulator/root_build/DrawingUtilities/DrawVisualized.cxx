
void DrawVisualized(const std::string& t_filename = "VisualizeDataRi35Simple.root")
{
	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);
	TFile *file = new TFile("VisualizeDataRi35Simple.root", "READ");
	TTree *tree = (TTree*)file->Get("ttree");

	double ms_min = 0.6, ms_max = 1, ms_dx =0.02;
	double mv_min = 0.6, mv_max = 1.1, mv_dx = 0.02;
	double S0_min = 25.7, S0_max = 36., S0_dx = 0.2;
	double L_min = 30., L_max = 120., L_dx = 2.;
	int S0_no = (int)((S0_max - S0_min)/S0_dx + 0.5);
	int L_no = (int)((L_max - L_min)/L_dx + 0.5);

	// divide canvas
	TCanvas *c1 = new TCanvas;
	// values of ms to loop through
	std::vector<double> ms, mv;
	for(double val = ms_min; val < ms_max; val += 5*ms_dx) ms.push_back(val);
	for(double val = mv_min; val < mv_max; val += 5*mv_dx) mv.push_back(val);
	c1->Divide(ms.size(), mv.size());

	unsigned canvas_id = 1;
	for(auto ms_it = ms.begin(); ms_it != ms.end(); ++ms_it)
		for(auto mv_it = mv.begin(); mv_it != mv.end(); ++mv_it)
		{
			c1->cd(canvas_id);
			std::string name = "hist" + to_string(canvas_id);
			tree->Draw(("S0:L>>" + name + "(" + to_string(L_no) + ","
				+ to_string(L_min) + "," + to_string(L_max) + ","
				+ to_string(S0_no) + "," + to_string(S0_min) + ","
				+ to_string(S0_max) + ")").c_str(), 
				("val*(index == 0)*(fabs( ms - " + to_string(*ms_it) + 
				") < 0.1)*(fabs( mv - " + to_string(*mv_it) + ") < 0.1)").c_str(), "colz");
			++canvas_id;
			c1->Modified();
			gPad->WaitPrimitive();
		}
	
	
}
