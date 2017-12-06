// STL
#include<cmath>
#include<vector>
#include<iostream>
#include<iomanip>
#include<fstream>

// ROOT
#include "TGraph.h"

#include "TExtraction.cxx"
#include "TExtraction.h"

using namespace std;

string path_112;
string path_124;
const Int_t numPoints = 10;
double SN112EK[numPoints];
double err_SN112EK[numPoints];
double SN124EK[numPoints];
double err_SN124EK[numPoints];
double DREK[13];
double err_DREK[13];


/*string sSN112EK[numPoints] = {"SN112EK12.5", "SN112EK17.5", "SN112EK22.5", "SN112EK27.5", "SN112EK32.5", "SN112EK37.5", "SN112EK42.5", "SN112EK47.5",
    "SN112EK52.5", "SN112EK57.5", "SN112EK62.5", "SN112EK67.5", "SN112EK72.5", "SN112EK77.5", "SN112EK82.5", "SN112EK87.5"};
string sSN124EK[numPoints] = {"SN124EK12.5", "SN124EK17.5", "SN124EK22.5", "SN124EK27.5", "SN124EK32.5", "SN124EK37.5", "SN124EK42.5", "SN124EK47.5",
    "SN124EK52.5", "SN124EK57.5", "SN124EK62.5", "SN124EK67.5", "SN124EK72.5", "SN124EK77.5", "SN124EK82.5", "SN124EK87.5"};
 */

string root_path = "/Users/pierremorfouace/Physics/EOS/ImQMD/Calculation/Data/Sn_MSL/e120/new_distribution";
double Ener_Rnp[numPoints] = {42.5, 47.5, 52.5, 57.5, 62.5, 67.5, 72.5, 77.5, 82.5, 87.5};
double Ener_DRnp[13] = {12.5, 17.5, 22.5, 27.5, 32.5, 37.5, 42.5, 47.5, 52.5, 60, 70, 80, 87.5};

string tokenRnp_112[20] = {"SN112EK42.5", "SN112EK47.5", "SN112EK52.5", "SN112EK57.5", "SN112EK62.5", "SN112EK67.5", "SN112EK72.5", "SN112EK77.5", "SN112EK82.5", "SN112EK87.5"};
string tokenRnp_124[20] = {"SN124EK42.5", "SN124EK47.5", "SN124EK52.5", "SN124EK57.5", "SN124EK62.5", "SN124EK67.5", "SN124EK72.5", "SN124EK77.5", "SN124EK82.5", "SN124EK87.5"};
string tokenDRnp[13] = {"DREK12.5","DREK17.5","DREK22.5","DREK27.5","DREK32.5","DREK37.5","DREK42.5","DREK47.5","DREK52.5","DREK60.0","DREK70.0","DREK80.0","DREK87.5"};


ofstream output;
ofstream output_par;

vector<int>    set_val;
vector<double> S0_val;
vector<double> L_val;
vector<double> ms_val;
vector<double> mv_val;
vector<double> fI_val;

void ExtractPosterior()
{
    vector<string> StringConfigFile;
    int set;
    double S0;
    double L;
    double ms;
    double mv;
    double fI;
    ifstream configfile;
    string configfile_name = "ConfigFile.dat";
    configfile.open(configfile_name);
    if(!configfile.is_open()) cout << "Config File not found !" << endl;
    cout << "****************" << endl;
    cout << "set S0 L  ms  fI" << endl;
    cout << "****************" << endl;
    while(!configfile.eof()){
        configfile >> set >> S0 >> L >> ms >> mv;
        fI = 1./ms-1./mv;
        cout << set << " " << S0 << " " << L << " " << ms << " " << mv << endl;
        set_val.push_back(set);
        S0_val.push_back(S0);
        L_val.push_back(L);
        ms_val.push_back(ms);
        mv_val.push_back(mv);
        fI_val.push_back(fI);
    }
    cout << "**************************************" << endl;
    configfile.close();
    
    for(unsigned int i=0; i<set_val.size(); i++){
        path_112 = root_path+"/sn112sn112/" + Form("%03d", i);
        path_124 = root_path+"/sn124sn124/" + Form("%03d", i);
        
        Double_t *array;
        Double_t Rnp112[20];
        Double_t err_Rnp112[20];
        Double_t Rnp124[20];
        Double_t err_Rnp124[20];
        Double_t DR[20];
        Double_t err_DR[20];
        
        TExtraction* Extractor = new TExtraction();
        
        ///////////////// Extraction /////////////////
        array = Extractor -> Get_CI_Rnp(path_112);
        for (Int_t j = 0; j < 20; j++) {
            Rnp112[j] = array[j];
            err_Rnp112[j] = array[j+20];
        }
        
        array = Extractor -> Get_CI_Rnp(path_124);
        for (Int_t j = 0; j < 20; j++) {
            Rnp124[j] = array[j];
            err_Rnp124[j] = array[j+20];
        }
        
        array = Extractor -> Get_CI_DR(path_124, path_112);
        for (Int_t j = 0; j < 20; j++) {
            DR[j] = array[j];
            err_DR[j] = array[j+20];
        }
        //////////////////////////////////////////////
        
        /////////////
        // Rnp 112 //
        /////////////
        TGraph* g = new TGraph();
        TGraph* ge = new TGraph();
        
        Double_t E = 5;
        for (Int_t j = 0; j < 20; j++) {
            g -> SetPoint(j, E, Rnp112[j]);
            ge -> SetPoint(j, E, err_Rnp112[j]);
            E += 10;
        }
        
       
        for (Int_t j = 0; j < numPoints; j++) {
            SN112EK[j] = g->Eval(Ener_Rnp[j]);
            err_SN112EK[j] = ge->Eval(Ener_Rnp[j]);
        }
        
        /////////////
        // Rnp 124 //
        /////////////
        g = new TGraph();
        ge = new TGraph();
        
        E = 5;
        for (Int_t j = 0; j < 20; j++) {
            g -> SetPoint(j, E, Rnp124[j]);
            ge -> SetPoint(j, E, err_Rnp124[j]);
            E += 10;
        }

        for (Int_t j = 0; j < numPoints; j++) {
            SN124EK[j] = g->Eval(Ener_Rnp[j]);
            err_SN124EK[j] = ge->Eval(Ener_Rnp[j]);
        }
        
        ////////
        // DR //
        ////////
        g = new TGraph();
        ge = new TGraph();
        
        E = 5;
        for (Int_t j = 0; j < 20; j++) {
            g -> SetPoint(j, E, DR[j]);
            ge -> SetPoint(j, E, err_DR[j]);
            E += 10;
        }
        
        for (Int_t j = 0; j < 13; j++) {
            DREK[j] = g->Eval(Ener_DRnp[j]);
            err_DREK[j] = ge->Eval(Ener_DRnp[j]);
        }
        /////////////
        
        string result       = "/Users/pierremorfouace/Physics/MADAI/analysis_MSL/new_distribution/e120/model_output/";
        string parameter    = "/Users/pierremorfouace/Physics/MADAI/analysis_MSL/new_distribution/e120/model_output/";
        string run;
        
        run = Form("run0%03d", i + 1);
        
        result      += run;
        parameter   += run;
        
        result      += "/results.dat";
        parameter   += "/parameters.dat";
        
        cout << result << endl;
        //cout << endl;
        
        output_par.open(parameter);
        output_par << "S0" << setw(12) << S0_val[i] << endl;
        output_par << "L" << setw(12) << L_val[i] << endl;
        output_par << "ms" << setw(12) << ms_val[i] << endl;
        output_par << "fi" << setw(12) << fI_val[i] << endl;
        output_par.close();
        
        output.open(result);
        for (Int_t j = 0; j < numPoints; j++) {
            output << tokenRnp_112[j] << setw(12) << SN112EK[j] << setw(12) << err_SN112EK[j] << endl;
            if(err_SN112EK[j]==0) cout << err_SN112EK[j] << endl;
            
        }
        for (Int_t j = 0; j < numPoints; j++) {
            output << tokenRnp_124[j] << setw(12) << SN124EK[j] << setw(12) << err_SN124EK[j] << endl;
            if(err_SN124EK[j]==0) cout << err_SN112EK[j] << endl;
        }
        
        for (Int_t j = 0; j < 13; j++) {
            output << tokenDRnp[j] << setw(12) << DREK[j] << setw(12) << err_DREK[j] << endl;
        }
        
        output.close();
        
    }
}
