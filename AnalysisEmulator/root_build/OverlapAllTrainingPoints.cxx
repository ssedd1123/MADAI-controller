
TGraphErrors *graph_from_file(const std::string& t_filename, int t_color)
{
	// graphs for the experimental results
	TGraphErrors *result = new TGraphErrors;
	std::ifstream result_file(t_filename.c_str());
	if(!result_file.is_open()) cout << " file " << t_filename << " cannot be opened\n";
	std::string buffer;
	double value, error;
	int x = 0;
	while(result_file >> buffer >> value >> error)
	{
		int point_num = result->GetN();
		result->SetPoint(point_num, (double) x, value);
		result->SetPointError(point_num, 0, error);
		result->SetMarkerStyle(8);
		++x;
	}

	result->SetMarkerColor(t_color);
	result->SetLineColor(t_color);
	return result;
}

void DrawExpData(TCanvas *c1 = 0)
{
	if(c1) c1->cd();
	vector<TGraphErrors*> results;
	results.push_back(graph_from_file("../template/experimental_output/Ri50_Ri/results.dat_b04_06", 2));
	results.push_back(graph_from_file("../template/experimental_output/Ri50_Ri/results.dat_b06_088", 3));
	results.push_back(graph_from_file("../template/experimental_output/Ri50_Ri/results.dat_b08_10", 4));
	for(auto result : results)
	{
		result->SetMarkerSize(2);
		result->SetLineWidth(5);
		result->Draw("PLsame");
	}

	auto legend = new TLegend(0.6,0.6,0.9,0.9);
	legend->AddEntry(results[0], "b 0.4 - 0.6", "PL");
	legend->AddEntry(results[1], "b 0.6 - 0.8", "PL");
	legend->AddEntry(results[2], "b 0.8 - 1.0", "PL");
	legend->Draw();
}

void OverlapAllTrainingPoints()
{
	//RunMaster controller("/mnt/analysis/hira/tsangc/Ri50_Ri/RiE50", "Ri50_Ri/");
        RunMaster controller("/mnt/analysis/hira/tsangc/new_distribution/e120_spectra", "e120_sn112_spectra/");
	vector<TGraphErrors*> graphs;
	for(int i = controller.GetStartNo(); i < controller.GetEndNo(); ++i)
	{
		vector<Data> data = controller.GetVarFromRun(i);
		vector<double> par = controller.GetParFromRun(i);
		graphs.push_back(new TGraphErrors);
		for(int j = 0; j < data.size(); ++j)
		{
			int point_num = graphs.back()->GetN();
			graphs.back()->SetPoint(point_num, (double) j, data[j].val); 
			graphs.back()->SetPointError(point_num, 0., data[j].err);
		}
		graphs.back()->SetTitle("120 MeV/u data");
		graphs.back()->GetXaxis()->SetTitle("Index for variables");
		graphs.back()->SetMarkerStyle(8);

	/*	if(par[0] > 32) 
		{
			graphs.back()->SetMarkerColor(kRed);
			graphs.back()->SetLineColor(kRed);
		}
		else 
		{*/
			graphs.back()->SetMarkerColor(kBlue);
			graphs.back()->SetLineColor(kBlue);
		//}
	}

	TCanvas *c1 = new TCanvas;
	c1->SetLogy();
	for(int i = 0; i < graphs.size(); ++i)
	{
		graphs[i]->GetYaxis()->SetRangeUser(0., 6.5);
		graphs[i]->Draw((i == 0)? "APL":"PLsame");
	}
	
//	DrawExpData();
	graph_from_file("../template/experimental_output/e120_sn112_spectra/results.dat", 2)->Draw("PLsame");
}

