void HyperPara()
{
    RunMaster controller("/mnt/analysis/hira/tsangc/new_distribution/e120", "e120/");

    std::vector<double> vS0, vL, vms, vmv, vScales, vNuggets;
    double S0_tree, L_tree, ms_tree, mv_tree;
    for(double val = 25.7; val < 36; val += 1) vS0.push_back(val);
    for(double val = 30; val < 120; val +=10) vL.push_back(val);
    for(double val = 0.6; val < 1.0; val +=0.05) vms.push_back(val);
    for(double val = 0.3; val < 1.2; val +=0.1) vmv.push_back(val);
    for(double val = 0.001; val < 3; val += 0.2) 
    {
        vScales.push_back(val);
        vNuggets.push_back(val);
    }

    std::ofstream file("HyperPara.csv");
    file << "Scales,Nuggets,";
    std::vector<std::string> var_name, par_name;
    controller.GetVarName(var_name);
    controller.GetParName(par_name);
    file << par_name[0];
    for(unsigned int i = 1; i < par_name.size(); ++i)
        file << "," << par_name[i];
    file << "," << var_name[0];
    for(unsigned int i = 1; i < var_name.size(); ++i)
        file << "," << var_name[i];
    file << "," << var_name[0] << "_err";
    for(unsigned int i = 1; i < var_name.size(); ++i)
        file << "," << var_name[i] << "_err";
    file << ",LogLikelihood\n";
    
    std::vector<double> exp_result{1.06776,
        0.959527,
        0.917892,
        0.845912,
        0.859419,
        0.846378,
        0.827128,
        0.864899,
        0.856574,
        0.871913,
        1.39551,
        1.31291,
        1.24418,
        1.17227,
        1.19943,
        1.20996,
        1.16403,
        1.19579,
        1.30153,
        1.18554,
        1.16017,
        1.19611,
        1.21876,
        1.24978,
        1.25837,
        1.29789,
        1.30695,
        1.36829,
        1.35548,
        1.39072,
        1.41844,
        1.45102,
        1.3597};



    for(double Scales : vScales)
        for(double Nuggets : vNuggets)
        {
            controller.SetScales(0.9);
            controller.SetNuggets(0.03548);
            for(double S0 : vS0)
                for(double L : vL)
                    for(double ms : vms)
                        for(double mv : vmv)
                        {

                                std::cout << "\r S0 = " << S0 << " L = " << L << " ms = " << ms << " mv = " << mv << " Scales = " << Scales << " Nuggets = " << Nuggets;
                                file << Scales << "," << Nuggets << "," << S0 << "," << L << "," << ms << "," << mv;
                                std::vector<Data> data = controller.RunEmulator(std::vector<double>{S0, L, ms, mv});
                                double loglikelihood = 0;
                                for(int index = 0; index < data.size(); ++index)
                                {
                                    file << "," <<  data[index].val;
                                    loglikelihood += -0.5*log(2*M_PI)-0.5*log(data[index].err) - pow((data[index].val - exp_result[index])/(2.*data[index].err), 2);
                                }
                                for(int index = 0; index < data.size(); ++index)
                                    file << "," <<  data[index].err;
                                file << "," << loglikelihood << "\n";
                        }
        }
}

