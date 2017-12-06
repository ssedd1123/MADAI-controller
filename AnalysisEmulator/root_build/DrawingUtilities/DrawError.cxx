void DrawError(double noise = 0.0, double scale = 0.6)
{
	TFile *f = new TFile("../No_Needed.root", "READ");
	TTree *tree = (TTree*)f->Get("ttree");
	
	TLegend *legend = new TLegend(0.7,0.7,0.9,0.9);
	vector<TH1F*> hist;
	for(int no_var = 0; no_var < 4; ++no_var)
	{
		tree->Draw(("no_run>>htemp" + to_string(no_var)).c_str(), 
			("fabs(((original[" + to_string(no_var) + "] - mean[" + to_string(no_var) + "])/original[" + to_string(no_var) + "])*(noise == " + to_string(noise) + ")*(scale==" + to_string(scale) + "))").c_str(),
			"goff");
		hist.push_back((TH1F*)gDirectory->FindObject(("htemp" + to_string(no_var)).c_str()));
		hist.back()->SetMarkerStyle(3);
		hist.back()->SetMarkerColor((no_var + 2 == 5)? 6 : no_var + 2); // 5 is yellow which looks bad
	}
	hist[0]->GetXaxis()->SetTitle("N.O. of runs used in training");
	hist[0]->GetYaxis()->SetTitle("Relative error");
	hist[0]->SetTitle(("Noise = " + to_string(noise) + " Scale = " + to_string(scale)).c_str());


	TCanvas *c1 = new TCanvas;
	// need the find the max relative error among all 4 variables to set range
	double max = 0;
	for(unsigned int i = 0; i < hist.size(); ++i)
	{
		max = (hist[i]->GetMaximum() > max)? hist[i]->GetMaximum(): max;		

		if(i == 0) hist[i]->Draw("hist p");
		else hist[i]->Draw("hist p same");
		std::string var_name;
		switch(i)
		{
			case 0 : var_name = "S0"; break;
			case 1 : var_name = "L"; break;
			case 2 : var_name = "ms"; break;
			case 3 : var_name = "fI"; break;
		}
		legend->AddEntry(hist[i], var_name.c_str(), "p");
	}
	legend->Draw();
	hist[0]->GetYaxis()->SetRangeUser(0, 1.1*max);
	c1->Modified();
	c1->Update();
}
