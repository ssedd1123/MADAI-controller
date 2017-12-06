

TVectorD fitlinear(RunMaster& controller, int t_var_index)
{
//	std::vector<TGraph*> graph;
	

	std::vector<std::string> par_name;
	controller.GetParName(par_name);
//	for(int i = 0; i < par_name.size(); ++i)
//		graph.push_back(new TGraph);

	
/*	for(int i = controller.GetStartNo(); i < controller.GetEndNo(); ++i)
	{
		std::vector<double> par = controller.GetParFromRun(i);
		std::vector<Data> var = controller.GetVarFromRun(i);
		for(int j = 0; j < graph.size(); ++j)
			graph[j]->SetPoint(graph[j]->GetN(), par[j], var[t_var_index].val);
	}

	std::vector<TCanvas*> c1;
	for(int i = 0; i < par_name.size(); ++i)
	{
		c1.push_back(new TCanvas);
		graph[i]->Draw("AP");
		graph[i]->GetXaxis()->SetTitle(par_name[i].c_str());
		graph[i]->GetYaxis()->SetTitle("value");
		graph[i]->SetMarkerStyle(8);
		c1.back()->Modified();
		c1.back()->Update();
	}
*/
	 TLinearFitter *lf=new TLinearFitter(5);

   //The predefined "hypN" functions are the fastest to fit
   lf->SetFormula("hyp4");

   Int_t n = 49;

   Double_t *x=new Double_t[n*10*4];
   Double_t *y=new Double_t[n*10];
   Double_t *e=new Double_t[n*10];

   //Create the points and put them into the fitter
   for (int i=0; i<n; i++){
      std::vector<double> par = controller.GetParFromRun(i + 1);
      x[0 + i*4] = par[0];
      x[1 + i*4] = par[1];
      x[2 + i*4] = par[2];
      x[3 + i*4] = par[3];
      e[i] = 0.01;
      std::vector<Data> var = controller.GetVarFromRun(i + 1);
      y[i] = var[t_var_index].val;
   }

   //To avoid copying the data into the fitter, the following function can be used:
   lf->AssignData(n, 4, x, y, e);
   //A different way to put the points into the fitter would be to use
   //the AddPoint function for each point. This way the points are copied and stored
   //inside the fitter

   //Perform the fitting and look at the results
   lf->Eval();
   TVectorD params;
   TVectorD errors;
   lf->GetParameters(params);
  /* lf->GetErrors(errors);
   for (Int_t i=0; i<5; i++)
      printf("par[%d]=%f+-%f\n", i, params(i), errors(i));
   Double_t chisquare=lf->GetChisquare();
   printf("chisquare=%f\n", chisquare);

	std::vector<TGraph*> linear_results;
	for(int i = 0; i < graph.size(); ++i) linear_results.push_back(new TGraph);

	for(int j = controller.GetStartNo(); j < controller.GetEndNo(); ++j)
	{
		std::vector<double> par = controller.GetParFromRun(j);
		double y = params(0);
		for(int k = 0; k < par_name.size(); ++k)
			y += params(k + 1)*par[k];

		cout << " y " << y << endl;

		for(int i = 0; i < linear_results.size(); ++i)
			linear_results[i]->SetPoint(linear_results[i]->GetN(), par[i], y);
	}		
*/
/*	for(int i = 0; i < linear_results.size(); ++i)
	{
		c1[i]->cd();
		linear_results[i]->Draw("Psame");
		linear_results[i]->SetMarkerColor(kRed);
		linear_results[i]->SetMarkerStyle(8);
		c1[i]->Modified();
		c1[i]->Update();
	}
*/
	return params;
}

void checklinear()
{
	RunMaster controller("/mnt/analysis/hira/tsangc/new_distribution/e120", "e120/");
	std::vector<std::string> var_name;
	controller.GetVarName(var_name);
	std::vector<TVectorD> all_params;
	for(int i = 0; i < 33; ++i)
		all_params.push_back(fitlinear(controller, i));

	TGraph *graph = new TGraph;
	for(int i = 1; i < 50; ++i)
	{
		std::vector<double> par = controller.GetParFromRun(i);
		std::vector<Data> var = controller.GetVarFromRun(i);
		for(int j = 0; j < 33; ++j)
		{
			double val = all_params[j](0);
			for(int k = 0; k < 4; ++k)
				val += all_params[j](k + 1)*par[k];

			graph->SetPoint(graph->GetN(), var[j].val, val);
		}
	}

	std::ofstream hyperplane_parameters("hyperplane_parameters.dat");
	if(!hyperplane_parameters.is_open()) throw std::runtime_error("File not opened\n");

	hyperplane_parameters 	<< "Coefficients for hyperplane to fit 120 MeV data\n"
				<< "Variables value can be calculated by\n"
				<< "Value = para0 + S0*para1 + L*para2 + ms*para3 + fi*para4\n\n";

	hyperplane_parameters	<< "Variables\tpara0\t\tpara1\t\tpara2\t\tpara3\t\tpara4\n";
	for(int i = 0; i < 33; ++i)
	{
		hyperplane_parameters << var_name[i];
		for(int j = 0; j < 5; ++j)
			hyperplane_parameters << std::setw(4) << "\t" << all_params[i](j);
		hyperplane_parameters << "\n";
	}
	

	graph->GetXaxis()->SetTitle("ImQMD result");
	graph->GetYaxis()->SetTitle("Linear hyperplane result");

	graph->SetMarkerStyle(8);
	graph->Draw("AP");
}
