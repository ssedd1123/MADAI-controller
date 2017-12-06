// stl
#include<cmath>
#include<vector>

#ifndef _TEXTRACTION_cxx
#define _TEXTRACTION_cxx

#include "TExtraction.h"

////////////////////////////////////////////////////////////////////////
double* TExtraction::Get_CI_DR(string path_to_DR_124, string path_to_DR_112)
{
    double *DR = new double[40];
//    double *err_DR = new double[20];
    
	path_to_DR_124 += "/NP-EK-A16Z6.DAT";
	path_to_DR_112 += "/NP-EK-A16Z6.DAT";

	ReadFile_124(path_to_DR_124);
	ReadFile_112(path_to_DR_112);

	for(int i=0; i<20; i++){
		DR[i] = Rnp_124[i]/Rnp_112[i];
//		err_DR[i] = sqrt( err_Rnp_124[i]*err_Rnp_124[i] + err_Rnp_112[i]*err_Rnp_112[i] );
		DR[i+20] = sqrt( err_Rnp_124[i]*err_Rnp_124[i]/(Rnp_112[i]*Rnp_112[i]) + Rnp_124[i]*Rnp_124[i]/pow(Rnp_112[i], 4)*err_Rnp_112[i]*err_Rnp_112[i] );
  }

	return DR;
}

////////////////////////////////////////////////////////////////////////
double* TExtraction::Get_CI_Rpp(string path_to_DR_124, string path_to_DR_112)
{
    double *R = new double[40];
    //double *err_R = new double[20];
    
    path_to_DR_124 += "/NP-EK-A16Z6.DAT";
    path_to_DR_112 += "/NP-EK-A16Z6.DAT";
    
    ReadFile_124(path_to_DR_124);
    ReadFile_112(path_to_DR_112);
    
    for(int i=0; i<20; i++){
        R[i] = Rp_124[i]/Rp_112[i];
        //err_R[i] = sqrt( err_Rp_124[i]*err_Rp_124[i] + err_Rp_112[i]*err_Rp_112[i] );
        R[i+20] = sqrt( pow(err_Rp_124[i]/Rp_112[i],2) + pow(Rp_124[i]*err_Rp_112[i],2)/pow(Rp_112[i],4) );
    
    }
    return R;
}

////////////////////////////////////////////////////////////////////////
double* TExtraction::Get_CI_Rnn(string path_to_DR_124, string path_to_DR_112)
{
    double *R = new double[20];
    double *err_R = new double[20];
    
    path_to_DR_124 += "/NP-EK-A16Z6.DAT";
    path_to_DR_112 += "/NP-EK-A16Z6.DAT";
    
    ReadFile_124(path_to_DR_124);
    ReadFile_112(path_to_DR_112);
    
    for(int i=0; i<20; i++){
        R[i] = Rn_124[i]/Rn_112[i];
        err_R[i] = sqrt( err_Rn_124[i]*err_Rn_124[i] + err_Rn_112[i]*err_Rn_112[i] );
    }
    return R;
}

////////////////////////////////////////////////////////////////////////
double* TExtraction::Get_CI_Rnp(string path_to_DR_124)
{
    double *R = new double[40];
//    double *err_R = new double[20];
    
    path_to_DR_124 += "/NP-EK-A16Z6.DAT";
    
    ReadFile_124(path_to_DR_124);
    
    for(int i=0; i<20; i++){
//        R[i] = Rn_124[i]/Rp_124[i];
//        err_R[i] = sqrt( err_Rn_124[i]*err_Rn_124[i] + err_Rp_124[i]*err_Rp_124[i] );
        R[i] = Rnp_124[i];
        R[i + 20] = err_Rnp_124[i];
    }
    return R;
}

////////////////////////////////////////////////////////////////////////
double* TExtraction::Get_Particle_Spectrum(string path_to_file, string particle, int system)
{
    double* R = new double[40];
    path_to_file += "/NP-EK-A16Z6.DAT";
    if(particle=="proton"){
        if(system==124){
            ReadFile_124(path_to_file);
            for(int i=0; i<20; i++){
                R[i] = Rp_124[i];
                R[i+20] = err_Rp_124[i];
            }
        }
        
        if(system==112){
            ReadFile_112(path_to_file);
            for(int i=0; i<20; i++){
                R[i] = Rp_112[i];
                R[i+20] = err_Rp_112[i];
            }
        }
    }
    
    if(particle=="neutron"){
        if(system==124){
            ReadFile_124(path_to_file);
            for(int i=0; i<20; i++){
                R[i] = Rn_124[i];
                R[i+20] = err_Rn_124[i];
            }
        }
        
        if(system==112){
            ReadFile_112(path_to_file);
            for(int i=0; i<20; i++){
                R[i] = Rn_112[i];
                R[i+20] = err_Rn_124[i];
            }
        }
    }
    
    return R;
}


////////////////////////////////////////////////////////////////////////
void TExtraction::ReadFile_124(string file_path)
{
	double E;
	double b;

	ifstream input_file;
	input_file.open(file_path.c_str());

	if(!input_file.is_open()) cout << "File " << file_path << " not found !" << endl;
	//else cout << "Opening file: " << file_path << endl;

	for(int i = 0; i<20; i++){
		input_file >> E >> b >> Ecm[i] >> Rp_124[i] >> err_Rp_124[i] >> Rn_124[i] >> err_Rn_124[i] >> Rnp_124[i] >> err_Rnp_124[i];
	}
	input_file.close();
	
	return;
}

////////////////////////////////////////////////////////////////////////
void TExtraction::ReadFile_112(string file_path)
{
	double E;
    double b;

	ifstream input_file;
	input_file.open(file_path.c_str());

	if(!input_file.is_open()) cout << "File " << file_path << " not found !" << endl;
	//else cout << "Opening file: " << file_path << endl;

	for(int i = 0; i<20; i++){
		input_file >> E >> b >> E >> Rp_112[i] >> err_Rp_112[i] >> Rn_112[i] >> err_Rn_112[i] >> Rnp_112[i] >> err_Rnp_112[i];
	}
	input_file.close();

	return;
}

////////////////////////////////////////////////////////////////////////
vector<double> TExtraction::Get_Ri(string root_path)
{
    vector<double> Ri;
    
    string path112 = root_path+"/sn112sn112e120b6x-1/ZMAX20-A.DAT";
    string path124 = root_path+"/sn124sn124e120b6x-1/ZMAX20-A.DAT";
    string path112_124 = root_path+"/sn112sn124e120b6x-1/ZMAX20-A.DAT";
    string path124_112 = root_path+"/sn124sn112e120b6x-1/ZMAX20-A.DAT";
    
    double Delta_124 = Get_Delta(path124);
    double Delta_112 = Get_Delta(path112);
    double Delta_124_112 = Get_Delta(path124_112);
    double Delta_112_124 = Get_Delta(path112_124);
    
    Ri.push_back(2*(Delta_124 - (Delta_124+Delta_112)/2)/(Delta_124 - Delta_112));//124
    Ri.push_back(2*(Delta_112_124 - (Delta_124+Delta_112)/2)/(Delta_124 - Delta_112));//112_124
    Ri.push_back(2*(Delta_124_112 - (Delta_124+Delta_112)/2)/(Delta_124 - Delta_112));//124_112
    Ri.push_back(2*(Delta_112 - (Delta_124+Delta_112)/2)/(Delta_124 - Delta_112));//112
    
    //for(int i=0; i<4; i++){
      //  cout << "Ri = " << Ri.at(i) << endl;
    //}
    return Ri;
}

////////////////////////////////////////////////////////////////
double TExtraction::Get_Delta(string path)
{
    double delta;
    
    ifstream file;
    file.open(path.c_str());
    
    if(!file.is_open()) cout << "File " << path << " not found !" << endl;
    
    double evt;
    double Z;
    double A;
    double ok;
    vector<double> v_Z;
    vector<double> v_N;
    
    while(file >> evt >> Z >> A >> ok){
        v_Z.push_back(Z);
        v_N.push_back(A-Z);
    }
    file.close();
    
    double mean_Z=0;
    double mean_N=0;
    for(unsigned int i=0; i<v_Z.size(); i++){
        mean_Z += v_Z.at(i);
        mean_N += v_N.at(i);
    }
    mean_Z = mean_Z/v_Z.size();
    mean_N = mean_N/v_Z.size();
    
    delta = (mean_N-mean_Z)/(mean_N+mean_Z);
    
    return delta;
}

#endif
