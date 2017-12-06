#include "OverlapAllTrainingPoints.cxx"

TCanvas* DrawConfig(RunMaster& controller, const std::vector<int>& run_num_within_cut, int t_color = 1, TCanvas *c1 = 0);
/*TGraph *graph_from_file(const std::string& t_filename, int t_color)
{
    // graphs for the experimental results
    TGraph *result = new TGraph;
    std::ifstream result_file(t_filename.c_str());
    if(!result_file.is_open()) cout << " file " << t_filename << " cannot be opened\n";
    std::string buffer;
    double value, error;
    int x = 0;
    while(result_file >> buffer >> value >> error)
    {
        result->SetPoint(result->GetN(), (double) x, value);
        result->SetMarkerStyle(8);
        ++x;
    }

    result->SetMarkerColor(t_color);
    result->SetLineColor(t_color);
    return result;
}*/

namespace config
{
    std::vector<double> par_max{36,120,1,0.66,10};
    std::vector<double> par_min{25.7,32,0.6,-0.59,3};
};

void LabelRuns(const std::vector<int>& t_excluded)
{
	RunMaster controller("/mnt/analysis/hira/tsangc/new_distribution/e120", "e120/");
	
	std::vector<int> included;
	for(int i = controller.GetStartNo(); i < controller.GetEndNo(); ++i)
		if(std::find(t_excluded.begin(), t_excluded.end(), i) == t_excluded.end()) included.push_back(i);

	TCanvas *c = DrawConfig(controller, included);
	DrawConfig(controller, t_excluded, 2, c);
}

void EmulatorVsImQMD(double t_scale = 0.01, double t_nuggets = 0.001)
{
	RunMaster controller("/mnt/analysis/hira/tsangc/new_distribution/e120", "e120_sn112_spectra/");
	controller.SetScales(t_scale);
	controller.SetNuggets(t_nuggets);
	
	const int set_excluded = 5;

	std::vector<TGraphErrors*> vec_graphs;
	TH1F *noise = new TH1F("noise", "noise", 1000, -0.5, 0.5);
	// exclude and see how the controller emulates
	for(int max_excluded = controller.GetStartNo(); max_excluded < controller.GetEndNo();)
	{
		std::vector<int> excluded;
		for(int i = 0; max_excluded < controller.GetEndNo() && i < set_excluded; ++i, ++max_excluded)
		{
			cout << "Excluded " << i << " Max " << max_excluded << endl;
			excluded.push_back(max_excluded);
		}

		cout << "Max " << max_excluded << endl;

		controller.ExcludeRun(excluded);
		for(auto run_num : excluded)
		{
			vec_graphs.push_back(new TGraphErrors);
			auto imqmd_result = controller.GetVarFromRun(run_num);
			auto emu_result = controller.RunEmulator(controller.GetParFromRun(run_num));
			for(unsigned int i = 0; i < emu_result.size(); ++i)
			{
				vec_graphs.back()->SetPoint(i, imqmd_result[i].val, emu_result[i].val);
				vec_graphs.back()->SetPointError(i, imqmd_result[i].err, emu_result[i].err);
				noise->Fill(emu_result[i].val - imqmd_result[i].val);
			}
		}
	}


	auto c2 = new TCanvas;
        c2->cd();
	noise->Draw();	

	auto c1 = new TCanvas;
	c1->cd();
	TMultiGraph *mg = new TMultiGraph;
	for(unsigned int i = 0; i < vec_graphs.size(); ++i)
	{
		vec_graphs[i]->SetMarkerStyle(8);
		mg->Add(vec_graphs[i]);
		//vec_graphs[i]->Draw((i==0)? "AP": "Psame");
	}

	mg->Draw("AP");
	mg->Fit("pol1", "FQ");
		
	auto xaxis = mg->GetXaxis();
	auto yaxis = mg->GetYaxis();

	xaxis->CenterTitle();
	xaxis->SetTitleSize(0.09);
	xaxis->SetLabelSize(0.09);
	xaxis->SetTitleOffset(0.9);
	xaxis->SetTitle("Left out points result");

	yaxis->CenterTitle();
	yaxis->SetTitleSize(0.09);
	yaxis->SetLabelSize(0.09);
	yaxis->SetTitleOffset(0.7);
	yaxis->SetTitle("Emulator result");

	c1->SaveAs("~/public_html/test.pdf");

	c1->Modified();
	c1->Update();
}


void ConfigurationsWithCutOnVariables(TCutG *t_cut)
{
    // cut on variables below which the configuration location will be shown
    //std::vector<double> variables_cut{0.1, 0.2, 0.3, 0.35, 0.4, 0.45};

    // put the cut in a graph
    //TGraph *cut_graph = new TGraph;
    //for(unsigned int i = 0; i < variables_cut.size(); ++i)
    //    cut_graph->SetPoint(cut_graph->GetN(), (double) i, variables_cut[i]);
    

    RunMaster controller("/mnt/analysis/hira/tsangc/Ri50_Ri/RiE50", "Ri50_Ri/");
    vector<TGraph*> graphs;
    vector<int> run_num_within_cut;
    for(int i = controller.GetStartNo(); i < controller.GetEndNo(); ++i)
    {
        bool within_cut = true;
        vector<Data> data = controller.GetVarFromRun(i);
        // if any points are not below cut, it is not within cut
        for(int j = 0; j < data.size(); ++j)
            if(t_cut)
                if(!t_cut->IsInside((double)j, data[j].val)/*data[j].val > variables_cut[j]*/) within_cut = false;

        if(within_cut)
        {
            graphs.push_back(new TGraph);
            for(int j = 0; j < data.size(); ++j)
                graphs.back()->SetPoint(graphs.back()->GetN(), (double) j, data[j].val); 
            run_num_within_cut.push_back(i);
        }
    }

    DrawConfig(controller, run_num_within_cut);

    TCanvas *c2 = new TCanvas;
    c2->cd();
    for(int i = 0; i < graphs.size(); ++i)
        graphs[i]->Draw((i == 0)? "APL":"PLsame");
    graphs[0]->GetYaxis()->SetRangeUser(0., 1);
    graphs[0]->GetYaxis()->SetTitle("Ri");
    graphs[0]->GetXaxis()->SetTitle("Index of variables");
    c2->Update();
    c2->Modified();

    DrawExpData(c2);

    /*graphs[0]->GetYaxis()->SetRangeUser(0, 1);

    vector<TGraph*> results;
    results.push_back(graph_from_file("../template/experimental_output/Ri35/results.dat_b04_06", 2));
    results.push_back(graph_from_file("../template/experimental_output/Ri35/results.dat_b06_088", 3));
    results.push_back(graph_from_file("../template/experimental_output/Ri35/results.dat_b08_10", 4));
    for(auto result : results)
    {
        result->SetMarkerSize(2);
        result->SetLineWidth(5);
        result->Draw("PLsame");
    }
*/
//    cut_graph->Draw("PLsame");

/*    auto legend = new TLegend(0.6,0.6,0.9,0.9);
    legend->AddEntry(results[0], "b 0.4 - 0.6", "PL");
    legend->AddEntry(results[1], "b 0.6 - 0.8", "PL");
    legend->AddEntry(results[2], "b 0.8 - 1.0", "PL");
    legend->Draw();
*/    
}

// adjust canvas margin such that graphs of matrix plot can stick together while leaving enough root for axis title
void AdjustCanvasMargin(TCanvas *c1, int par_num)
{
    Float_t small = 1e-5;
    Float_t larger = 0.2;
    c1->Divide(par_num, par_num, small, small);
    gStyle->SetPadBorderMode(0);
    gStyle->SetFrameBorderMode(0);

    for(unsigned int i = 0; i < par_num; ++i)
        for(unsigned int j = 0; j < par_num; ++j)
        {
            c1->cd(i + par_num*j + 1);
            if(i != par_num - 1) gPad->SetRightMargin(small);
            if(j != par_num - 1) gPad->SetBottomMargin(small);
            else gPad->SetBottomMargin(larger);
            if(i != 0) gPad->SetLeftMargin(small);
            else gPad->SetLeftMargin(larger);
            if(j != 0) gPad->SetTopMargin(small);
        }
}

// get parameter space
TCanvas* DrawConfig(RunMaster& controller, const std::vector<int>& run_num_within_cut, int t_color, TCanvas *c1)
{
    gStyle->SetOptStat(0);
    gStyle->SetPadBorderMode(0);
    gStyle->SetFrameBorderMode(0);
    Float_t small = 1e-5;
    Float_t larger = 0.2;


    std::vector<std::string> par_name;
    controller.GetParName(par_name);
    unsigned int par_num = par_name.size();
    bool provided_canvas = true;

    if(!c1) 
    {
        c1 = new TCanvas;
	//c1->Divide(par_num, par_num, small, small);
        AdjustCanvasMargin(c1, par_num);
        provided_canvas = false;
    }

    std::vector<std::vector<TGraph*>> configurations(par_num, std::vector<TGraph*>(par_num));
    std::vector<TH1F*> configurations_1D;

    // initialize all graphs
    for(auto& vgraph : configurations)
        for(auto& graph : vgraph)
        {
            graph = new TGraph;
            graph->SetMarkerStyle(8);
            graph->SetMarkerColor(t_color);
        }

    // draw all graphs
    for(auto index : run_num_within_cut)
    {
        std::vector<double> par = controller.GetParFromRun(index);
        for(unsigned int i = 0; i < par.size(); ++i)
            for(unsigned int j = 0; j < par.size(); ++j)
            {
                if(i == j) continue;
                configurations[i][j]->SetPoint(configurations[i][j]->GetN(), par[i], par[j]);
            }
    }

    // draw them onto canvas
    for(unsigned int i = 0; i < par_num; ++i)
        for(unsigned int j = 0; j < par_num; ++j)
        {
            if(i == j) continue;
            c1->cd(i + par_num*j + 1);
     /*       if(i != par_num - 1) gPad->SetRightMargin(small);
            if(j != par_num - 1) gPad->SetBottomMargin(small);
            else gPad->SetBottomMargin(larger);
            if(i != 0) gPad->SetLeftMargin(small);
            else gPad->SetLeftMargin(larger);
            if(j != 0) gPad->SetTopMargin(small);*/
            configurations[i][j]->GetXaxis()->SetTitle(par_name[i].c_str());
            configurations[i][j]->GetXaxis()->SetRangeUser(config::par_min[i], config::par_max[i]);
            configurations[i][j]->GetXaxis()->CenterTitle();
            configurations[i][j]->GetXaxis()->SetTitleSize(0.09);
            configurations[i][j]->GetXaxis()->SetLabelSize(0.09);
            configurations[i][j]->GetXaxis()->SetTitleOffset(0.75); 
            configurations[i][j]->GetYaxis()->SetTitle(par_name[j].c_str());
            configurations[i][j]->GetYaxis()->SetRangeUser(config::par_min[j], config::par_max[j]);
            configurations[i][j]->Draw((provided_canvas)? "Psame" : "AP");
            configurations[i][j]->GetYaxis()->CenterTitle();
            configurations[i][j]->GetYaxis()->SetTitleSize(0.09);
            configurations[i][j]->GetYaxis()->SetLabelSize(0.09);
            configurations[i][j]->GetYaxis()->SetTitleOffset(0.75);
        }

    // project those graphs to form a 1D distribution for each variable
    for(unsigned int i = 0; i < par_num; ++i)
    {
        // graphs to be projected
        TGraph* to_be_projected_y = (i==0)? configurations[1][0] : configurations[0][i];
        // find the range of the parameters
        double xmin = to_be_projected_y->GetYaxis()->GetXmin();
        double xmax = to_be_projected_y->GetYaxis()->GetXmax();
        c1->cd(i + i*par_num + 1);
        configurations_1D.push_back(new TH1F(("par " + to_string(i)).c_str(), "",100, xmin, xmax));
        for(unsigned int j = 0; j < to_be_projected_y->GetN(); ++j)
        {
            double val, temp;
            to_be_projected_y->GetPoint(j, temp, val);
            configurations_1D.back()->Fill(val);
        }
 /*       if(i != par_num - 1) gPad->SetRightMargin(small);
        if(i != par_num - 1) gPad->SetBottomMargin(small);
        else gPad->SetBottomMargin(larger);
        if(i != 0) gPad->SetLeftMargin(small);
        else gPad->SetLeftMargin(larger);
        if(i != 0) gPad->SetTopMargin(small);
*/
        configurations_1D.back()->GetXaxis()->SetTitleSize(0.09);
        configurations_1D.back()->GetXaxis()->SetLabelSize(0.09);
        configurations_1D.back()->GetXaxis()->SetTitleOffset(0.75); 
        configurations_1D.back()->GetXaxis()->SetTitle(par_name[i].c_str());
        configurations_1D.back()->GetXaxis()->CenterTitle();
        configurations_1D.back()->GetYaxis()->SetTitleSize(0.09);
        configurations_1D.back()->GetYaxis()->SetLabelSize(0.09);
        configurations_1D.back()->GetYaxis()->SetTitleOffset(0.75);
        configurations_1D.back()->SetLineColor(t_color);
        configurations_1D.back()->Draw((provided_canvas)? "same":"");
    }

    if(provided_canvas) return 0;
    else return c1;
}
