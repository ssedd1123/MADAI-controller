#include "loglikelihood.h"
#include "Math.h"

typedef std::chrono::high_resolution_clock Clock;

template<class Graph>
void CorrectAxisSize(Graph *graph, const std::string& t_xaxis, const std::string& t_yaxis, const std::string& t_title)
{
	graph->SetTitle(t_title.c_str());

	auto xaxis = graph->GetXaxis();
	auto yaxis = graph->GetYaxis();

	xaxis->SetTitle(t_xaxis.c_str());
	xaxis->CenterTitle();
	xaxis->SetTitleSize(0.09);
	xaxis->SetLabelSize(0.09);
	xaxis->SetTitleOffset(0.8);

	yaxis->SetTitle(t_yaxis.c_str());
	yaxis->CenterTitle();
	yaxis->SetTitleSize(0.09);
	yaxis->SetLabelSize(0.09);
	yaxis->SetTitleOffset(1.2);
}

void generate_graph(TGraph2D *graph, const std::vector<int>& t_test_set,  const std::vector<int>& t_exclude, 
	double scale_min = 0.1, double scale_max = 2., double scale_dx = 0.1,
	double nuggets_min = 0.01, double nuggets_max = 1., double nuggets_dx = 0.1,
	int t_thread_num = 0)
{
	std::vector<int> tot_excluded = t_exclude;
	tot_excluded.insert(tot_excluded.end(), t_test_set.begin(), t_test_set.end());
	std::sort(tot_excluded.begin(), tot_excluded.end());

	RunMaster controller("/mnt/analysis/hira/tsangc/AnalysisEmulator/DataCreator_mult/DataCreator" + to_string(t_thread_num), "DataCreator/");
	loglikelihood test(controller);//Ri50_Ri/RiE50_" + to_string(t_thread_num), "Ri50_Ri/"));
	test.ExcludeRun(tot_excluded);//std::vector<int>{1,2,3,4,5,6,7,8,9});

	//std::vector<std::string> var_name;
	//controller.GetVarName(var_name);
	//controller.SetObsList(std::vector<std::string>(var_name.begin(), var_name.end() - 2));
	
	//TGraph2D *graph = new TGraph2D;
	for(double scale = scale_min; scale < scale_max; scale += scale_dx)
		for(double nuggets = nuggets_min; nuggets < nuggets_max; nuggets += nuggets_dx)
	{
		auto t1 = Clock::now();
		test.Validation(scale, nuggets, t_exclude);
		graph->SetPoint(graph->GetN(), scale, nuggets, test.GetLogProb());
		graph->GetXaxis()->SetTitle("scale");
		graph->GetYaxis()->SetTitle("nuggets");
		auto t2 = Clock::now();
		cout << " finished scale " << scale << " nuggets " << nuggets << " " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " milliseconds " << std::endl;
	}
//	graph->Draw("colz");
//	gPad->WaitPrimitive();
}

TGraph2D* loglikelihood_graph(const std::vector<int>& t_test_set, double scale_min = 0.1, double scale_max = 1.5, double scale_dx = 0.1,
	double nuggets_min = 0.01, double nuggets_max = 1., double nuggets_dx = 0.1)
{
	const int num_graphs = 9;
	const int num_each_exclude = 10;
	const int num_max_runs = 100;
	std::vector<std::thread> th;
	vector<TGraph2D*> graph;

	int max_excluded = 0;
	for(int i = 0; i < num_graphs; ++i)
	{
		std::vector<int> exclude;
		for(; exclude.size() < num_each_exclude && max_excluded < num_max_runs; ++max_excluded)
			if(std::find(t_test_set.begin(), t_test_set.end(), max_excluded) == t_test_set.end())
				exclude.push_back(max_excluded);

		if(exclude.size() == 0) continue;

		graph.push_back(new TGraph2D);
		th.push_back(std::thread(generate_graph, graph.back(), t_test_set, exclude,  scale_min, scale_max, scale_dx, nuggets_min, nuggets_max, nuggets_dx, i + 1));
	}
		
	for(auto& t : th)
		t.join();

	TGraph2D *sum = new TGraph2D;
	Double_t *x = graph.back()->GetX();
	Double_t *y = graph.back()->GetY();
	std::vector<Double_t*> z;

	for(int j = 0; j < graph.size(); ++j)
		z.push_back(graph[j]->GetZ());

	for(unsigned int i = 0; i < graph.back()->GetN(); ++i)
	{
		double val = 0;
		std::cout << "start " << i << endl;
		for(int j = 0; j < graph.size(); ++j)
			val += z[j][i];
	//	cout << x[i] << " " << y[i] << " " << val << endl;
		sum->SetPoint(i, x[i], y[i], val);
	//	cout << i << " " << graph.back()->GetN() << endl;
	}

	//cout << sum->GetN() << endl;
	//sum->Draw("colz");
	return sum;

}

void generate_value(double &t_prob, loglikelihood& test, std::vector<int>& t_exclude, double scale = 0.1, double nuggets = 0.1)
{
	//TGraph2D *graph = new TGraph2D;
	test.Validation(scale, nuggets, t_exclude);
	t_prob = test.GetLogProb();
}

void ValidationOnTestSet(const std::string& t_type = "DataCreator", const std::vector<double>& t_point = std::vector<double>{0.4, 0.4}, const double speed = 5e-4, const int num_each_exclude = 7, const std::vector<int>& test_set = std::vector<int>{1,2,3,4,5}, TGraph2D *likelihood_graph = 0)
{
	gStyle->SetPalette(kBird);
	TCanvas *c1 = new TCanvas;
	TCanvas *c2 = new TCanvas;
	c1->SetBottomMargin(0.2);
	c2->SetBottomMargin(0.2);
	c1->SetLeftMargin(0.2);
	c2->SetLeftMargin(0.2);
	gPad->Modified();
	gPad->Update();
	
	std::string type, dir_single, dir_mult;
	if(t_type == "DataCreator")
	{
		type = "DataCreator/";
		dir_single = "/mnt/analysis/hira/tsangc/AnalysisEmulator/DataCreator";
		dir_mult = dir_single + "_mult/DataCreator";
	}
	else if(t_type == "e120")
	{
		type = "e120/";
		dir_single = "/mnt/analysis/hira/tsangc/new_distribution/e120";
		dir_mult = dir_single + "_";
	}
	else if(t_type == "Ri50_Ri")
	{
		type = "Ri50_Ri/";
		dir_single = "/mnt/analysis/hira/tsangc/Ri50_Ri/RiE50";
		dir_mult = dir_single + "_";
	}
	else if(t_type == "e120_2systems")
	{
		type = "e120_2systems/";
		dir_single = "/mnt/analysis/hira/tsangc/new_distribution/e120";
		dir_mult = dir_single + "_";
	}
        else if(t_type == "e120_sn112_spectra")
	{
		type = "e120_sn112_spectra/";
		dir_single = "/mnt/analysis/hira/tsangc/new_distribution/e120";
		dir_mult = dir_single + "_";
	}
	else if(t_type == "e120_bugs_free")
	{
		type = "e120_bugs_free/";
		dir_single = "/mnt/analysis/hira/tsangc/new_distribution/e120";
		dir_mult = dir_single + "_";
	}
	else
		throw std::runtime_error("type not found");

	RunMaster controller(dir_single, type);
	const int num_graphs = 9;
	const int num_max_runs = controller.GetEndNo() - controller.GetStartNo() + 1;
	vector<loglikelihood> likelihood_vec;
	vector<vector<int>> vec_exclude;

	// we exclude 5 set as tests set
	// never use that in maximizing log likelihood. 
	// see if the optimied scale and nuggets can predict the new value


	int exclude_index = 0;

	std::vector<int> set_of_runs;

	for(int i = controller.GetStartNo(); i < controller.GetEndNo(); ++i)
		set_of_runs.push_back(i);

	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(set_of_runs.begin(), set_of_runs.end(), g);

	for(int i = 0; i < num_graphs; ++i)
	{
		std::vector<int> exclude;
		cout << " start exclude\n";
		for(; exclude.size() < num_each_exclude && exclude_index < set_of_runs.size() ; ++exclude_index)
			if(std::find(test_set.begin(), test_set.end(), set_of_runs[exclude_index]) == test_set.end())
			{
				cout << set_of_runs[exclude_index] << endl;
				exclude.push_back(set_of_runs[exclude_index]);
			}

		if(exclude.size() == 0) continue;
		vec_exclude.push_back(exclude);

		RunMaster controller(dir_mult + to_string(i + 1), type);

		likelihood_vec.emplace_back(loglikelihood(controller));//RunMaster(dir_mult + to_string(i + 1), type)));//AnalysisEmulator/DataCreator_mult/DataCreator" + to_string(i + 1), "DataCreator/")));//new_distribution/e120_" + to_string(i + 1), "e120/")));
		std::vector<int> tot_excluded = exclude;
		tot_excluded.insert(tot_excluded.end(), test_set.begin(), test_set.end());
		std::sort(tot_excluded.begin(), tot_excluded.end());
	
		likelihood_vec.back().ExcludeRun(tot_excluded);
	}

	// Generate log likelihood graph if it is not provided
//	if(!likelihood_graph) likelihood_graph = loglikelihood_graph(test_set);

	c1->cd();
//	likelihood_graph->Draw("colz");
	TGraph *point_chosen = new TGraph;
	std::vector<double> point = t_point;//{0.3,0.45};
	point_chosen->SetPoint(point_chosen->GetN(), point[0], point[1]);
	point_chosen->SetMarkerStyle(8);
	if(likelihood_graph) 
	{
		CorrectAxisSize(likelihood_graph, "Scale", "Nugget", "Scale and nuggets chosen by gradient descent");
		likelihood_graph->Draw("colz");
		point_chosen->Draw("PL same");
	}else
		point_chosen->Draw("APL");
	gPad->Modified();
	gPad->Update();

	c2->cd();
	TGraph *test_set_accuracy = new TGraph;
	TGraph *validation_set_loglikelihood_per_set = new TGraph;
	validation_set_loglikelihood_per_set->SetMarkerStyle(8);
	test_set_accuracy->SetMarkerStyle(8);
	test_set_accuracy->Draw("APL");

	TCanvas *c3 = new TCanvas;
	validation_set_loglikelihood_per_set->SetLineColor(kRed);
	validation_set_loglikelihood_per_set->Draw("APL");

	auto legend = new TLegend(0.7, 0.3, 0.9, 0.5);
	legend->AddEntry(test_set_accuracy, "Log likelihood per set for testing sets", "pl");
	legend->AddEntry(validation_set_loglikelihood_per_set, "Log likelihood per set for validation sets", "pl");
	legend->Draw();
	// calculate log likelihood for the test set against emulator's prediction
	// which is optimized with only validation set
	loglikelihood test(controller);//AnalysisEmulator/DataCreator", "DataCreator/"));//new_distribution/e120", "e120/"));
	test.ExcludeRun(test_set);
	//TGraph2D *graph = new TGraph2D;
	auto func2 = [&](const std::vector<double>& t_para)
	{
		double val = 0;
		std::vector<std::thread> th;
		std::vector<double> prob(likelihood_vec.size());
	
		for(int i = 0; i < likelihood_vec.size(); ++i)
			th.push_back(std::thread(generate_value, std::ref(prob[i]), std::ref(likelihood_vec[i]), std::ref(vec_exclude[i]), t_para[0], t_para[1]));
			
		for(auto& t : th)
			t.join();

		for(double result : prob)
			val += result;

		return val;
	};

	

	for(int counter = 0 ; counter < 45 ; ++counter)
	{
		
		c2->cd();
		point = Math::GradDescentStep(func2, point, 1e-3, speed);
		point_chosen->SetPoint(point_chosen->GetN(), point[0], point[1]);
		test.Validation(point[0], point[1]);
		test_set_accuracy->SetPoint(test_set_accuracy->GetN(), /*likelihood_graph->Interpolate(point[0], point[1])*/ (double) counter, test.GetLogProb() / (double) test_set.size());
		validation_set_loglikelihood_per_set->SetPoint(validation_set_loglikelihood_per_set->GetN(), (double) counter, func2(point) / (double) ( num_max_runs - 1 - test_set.size()));
		CorrectAxisSize(test_set_accuracy, "Iteration in gradient descent", "Log likelihood of the left out sets", "Validation with test sets");
		gPad->Modified();
		gPad->Update();
		c1->cd();
		CorrectAxisSize(point_chosen, "Scale", "Nugget", "Scale and nuggets chosen by gradient descent");
		gPad->Modified();
		gPad->Update();
		c3->cd();
		CorrectAxisSize(validation_set_loglikelihood_per_set, "Iteration in gradient descent", "Log likelihood of the left out sets", "Validation with test sets");
		gPad->Modified();
		gPad->Update();
		
		
	}

}
