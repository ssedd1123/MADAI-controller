void checkData() {
    gROOT->SetStyle("pierre_style");
    
    const Int_t numPoints = 33;
    const Int_t numSets = 49;
    
    ifstream file[numSets];
    for (auto i = 0; i < numSets; i++)
        file[i].open(Form("run0%03d/results.dat", i + 1));
    
    TString dummy;
    Double_t x[numPoints];
    Double_t x_Rnp[10];
    Double_t x_DRnp[13];
    Double_t val[numSets][numPoints];
    Double_t DR[numSets][13];
    Double_t R112[numSets][10];
    Double_t R124[numSets][10];
    Double_t dbdummy;
    Double_t val_data;
    
    for (auto i = 0; i < numSets; i++) {
        for (auto j = 0; j < numPoints; j++) {
            //file[i] >> dummy >> val[i][j] >> dbdummy;
            file[i] >> dummy >> val_data >> dbdummy;
            //cout << dummy << endl;
            //x[j] = dummy.ReplaceAll("SN112EK", "").Atof();
            
            if(j<10){
                R112[i][j] = val_data;
                x_Rnp[j] = dummy.ReplaceAll("SN112EK", "").Atof();
            }
            
            else if(j>9 && j<20){
                R124[i][j-10] = val_data;
            }
            
            else if(j>19){
                DR[i][j-20] = val_data;
                x_DRnp[j-20] = dummy.ReplaceAll("DREK", "").Atof();
                //cout <<  j-20 << " " << x_DRnp[j-20] << " " << DR[i][j-20] << endl;
            }
        }
    }
    
    TCanvas* c1 = new TCanvas("c1", "c1", 640, 480);
    for (int i = 0; i < numSets; i++) {
        TGraph* graphDR = new TGraph(13, x_DRnp, DR[i]);
        graphDR -> SetLineColor(4);
        if (i == 0) {
            graphDR -> Draw("AL");
            graphDR -> SetTitle("");
            graphDR -> GetXaxis() -> SetTitle("E_{c.m.} (MeV)");
            graphDR -> GetXaxis() -> CenterTitle();
            graphDR -> GetXaxis() -> SetLimits(10, 90);
            graphDR -> GetXaxis() -> SetRangeUser(0, 90);
            graphDR -> GetYaxis() -> SetTitle("DR_{n/p}");
            graphDR -> GetYaxis() -> CenterTitle();
            graphDR -> GetYaxis() -> SetLimits(0.85, 2.0);
            graphDR -> GetYaxis() -> SetRangeUser(1, 1.6);
            graphDR -> SetLineColor(4);
        } else
            graphDR -> Draw("Lsame");
    }
    
    
    TCanvas* c2 = new TCanvas("c2", "c2", 640, 480);
    for (int i = 0; i < numSets; i++) {
        TGraph* graphR = new TGraph(10, x_Rnp, R124[i]);
        graphR -> SetLineColor(4);
        if (i == 0) {
            graphR -> Draw("AL");
            graphR -> SetTitle("");
            graphR -> GetXaxis() -> SetTitle("E_{c.m.} (MeV)");
            graphR -> GetXaxis() -> CenterTitle();
            graphR -> GetXaxis() -> SetLimits(10, 90);
            graphR -> GetXaxis() -> SetRangeUser(40, 90);
            graphR -> GetYaxis() -> SetTitle("R_{n/p}");
            graphR -> GetYaxis() -> CenterTitle();
            graphR -> GetYaxis() -> SetLimits(0.85, 2.0);
            graphR -> GetYaxis() -> SetRangeUser(0.7, 2);
            graphR -> SetLineColor(4);
        } else
        graphR -> Draw("Lsame");
    }
    

    ifstream data("../experimental_data/results.dat");
    Double_t exp_data;
    Double_t err_exp_data;
   
    Double_t exp_DR[13];
    Double_t err_exp_DR[13];
    Double_t fake[10];

    for (auto i = 0; i < numPoints; i++){
        data >> dummy >> exp_data >> err_exp_data;
        
        if(i<10){
            fake[i] = exp_data;
            fake[i] = err_exp_data;
        }
        
        else if(i>9 && i<20){
            fake[i-10] = exp_data;
            fake[i-10] = err_exp_data;
        }
        
        else if(i>19){
            exp_DR[i-20] = exp_data;
            err_exp_DR[i-20] = err_exp_data;

        }

    }
    
    auto graph = new TGraphErrors(13, x_DRnp, exp_DR, nullptr, err_exp_DR);
    graph -> SetMarkerStyle(20);
    graph -> SetMarkerColor(2);
    graph -> SetMarkerSize(2.0);
    graph -> SetLineWidth(1);
    graph -> SetLineColor(2);
    c1->cd();
    graph -> Draw("Psame");
    
    Double_t exp_R112[7];
    Double_t exp_R124[7];
    Double_t err_exp_R112[7];
    Double_t err_exp_R124[7];
    Double_t x_R_exp[7];
    ifstream data_R124("/Users/pierremorfouace/Physics/MADAI/analysis_MSL/Rnp_allsets/sn124e120_40MeVcut/experimental_data/results.dat");
    for (auto i = 0; i < 7; i++){
        data_R124 >> dummy >> exp_R124[i] >> err_exp_R124[i];
        x_R_exp[i] = dummy.ReplaceAll("EK", "").Atof();
        
        cout << x_R_exp[i] << " " << exp_R124[i] << endl;
    }
    
    graph = new TGraphErrors(7, x_R_exp, exp_R124, nullptr, err_exp_R124);
    graph -> SetMarkerStyle(20);
    graph -> SetMarkerColor(2);
    graph -> SetMarkerSize(2.0);
    graph -> SetLineWidth(1);
    graph -> SetLineColor(2);
    c2->cd();
    graph -> Draw("Psame");
    
    
}
