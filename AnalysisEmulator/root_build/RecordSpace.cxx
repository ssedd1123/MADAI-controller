
void RecordSpace()
{
    TFile *file = new TFile("RecordSpace.root", "RECREATE");
    TTree *tree = new TTree;

    RunMaster controller("/projects/hira/MADAI/Tsangc/TestModelEmulate/TestModelEmulate", "TestModelEmulate/");
    controller.SetScales(0.9);
    controller.SetNuggets(0.03548);

    std::vector<double> vS0, vL, vms, vmv;
    double S0_tree, L_tree, ms_tree, mv_tree;
    for(double val = 25.7; val < 36; val += 0.5) vS0.push_back(val);
    for(double val = 30; val < 120; val +=5) vL.push_back(val);
    for(double val = 0.6; val < 1.0; val +=0.05) vms.push_back(val);
    for(double val = 0.3; val < 1.2; val +=0.05) vmv.push_back(val);


    std::vector<double> val(controller.GetVarFromRun(1).size());
    std::vector<double> err(val.size());
    tree->Branch("S0", &S0_tree);
    tree->Branch("L", &L_tree);
    tree->Branch("ms", &ms_tree);
    tree->Branch("mv", &mv_tree);
    tree->Branch("val", &val);
    tree->Branch("err", &err);
    
    for(double S0 : vS0)
        for(double L : vL)
            for(double ms : vms)
                for(double mv : vmv)
                {

                    std::cout << "\r S0 = " << S0 << " L = " << L << " ms = " << ms << " mv = " << mv ;
                    S0_tree = S0; L_tree = L; ms_tree = ms; mv_tree = mv;
                    std::vector<Data> data = controller.RunEmulator(std::vector<double>{S0, L, ms, mv});
                    for(int index = 0; index < data.size(); ++index)
                    {
                        val[index] = data[index].val;
                        err[index] = data[index].err;
                        tree->Fill();
                    }
                }
    tree->Write("ttree");

    // tree that stores the axis and bin size of each variables
    // will be usefule when we try to plot them in the future
    TTree *axisTree = new TTree;
    axisTree->Branch("S0", &vS0);
    axisTree->Branch("L", &vL);
    axisTree->Branch("ms", &vms);
    axisTree->Branch("mv", &vmv);
    axisTree->Fill();

    axisTree->Write("Axis");
}

std::string LSquareCutCommand(const std::vector<Data>& t_model)
{
	std::string cut;
	cut = " (pow (( val[0] - " + to_string(t_model[0].val) + "), 2)";
	for(unsigned int i = 1; i < t_model.size(); ++i)
		cut += " + pow (( val[" + to_string(i) + "] - " + to_string(t_model[i].val) + "), 2 ) ";

	cut += ")";
	return cut;
}

std::string ChiSquareCutCommand(const std::vector<Data>& t_model)
{
	std::string cut;
	cut = " (pow (( val[0] - " + to_string(t_model[0].val) + ")/err[0], 2)";
	for(unsigned int i = 1; i < t_model.size(); ++i)
		cut += " + pow (( val[" + to_string(i) + "] - " + to_string(t_model[i].val) + ")/err[" + to_string(i) + "], 2 ) ";

	cut += ")";
	return cut;
}

std::string LogLikelihoodCutCommand(const std::vector<Data>& t_model)
{
	std::string cut;
	cut = "(-0.5*log( err[0] ) ";
	for(unsigned int i = 1; i < t_model.size(); ++i)
		cut += " - 0.5*log( err[" + to_string(i) + "] )";

	cut += " - ";
	cut += ChiSquareCutCommand(t_model);
	cut += " )";

	return cut;
}

TH2F *HistForAxis(TTree *t_tree, TTree *t_content, const std::string& t_x, const std::string& t_y, const std::string& t_name, const std::string t_cut, const std::string t_option)
{
	TTreeReader myReader(t_tree);
	TTreeReaderValue<std::vector<double>> x_axis(myReader, t_x.c_str());
	TTreeReaderValue<std::vector<double>> y_axis(myReader, t_y.c_str());
	myReader.Next();
	std::vector<double> x(*x_axis);
	std::vector<double> y(*y_axis);
	std::sort(x.begin(), x.end());
	std::sort(y.begin(), y.end());
	TH2F *hist = new TH2F(t_name.c_str(), "", x.size() - 1, &x[0], y.size() - 1, &y[0]);
	t_content->Draw((t_y + ":" + t_x + ">>" + t_name).c_str(), t_cut.c_str(), t_option.c_str());
	return hist;
}
