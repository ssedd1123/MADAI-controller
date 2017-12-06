#ifndef _TEXTRACTION_
#define _TEXTRACTION_

// stl
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
using namespace std;

class TExtraction
{
public:
    TExtraction() {};
    ~TExtraction() {};

public:
    string path_to_DR_124;
    string path_to_DR_112;
    
    double Rp_124[20];
    double Rn_124[20];
    double Rp_112[20];
    double Rn_112[20];
    double err_Rp_124[20];
    double err_Rn_124[20];
    double err_Rp_112[20];
    double err_Rn_112[20];
    
    double Rnp_124[20];
    double err_Rnp_124[20];
    double Rnp_112[20];
    double err_Rnp_112[20];
    double Ecm[20];
    
    
public:
    double* Get_CI_DR(string path_to_DR_124, string path_to_DR_112);
    double* Get_CI_Rpp(string path_to_DR_124, string path_to_DR_112);
    double* Get_CI_Rnn(string path_to_DR_124, string path_to_DR_112);
    double* Get_CI_Rnp(string path_to_DR_124);
    double* Get_Particle_Spectrum(string path_to_file, string particle, int system);
    double Get_Delta(string path);
    vector<double> Get_Ri(string root_path);
    void ReadFile_124(string file_path);
    void ReadFile_112(string file_path);
    
    
};

#endif
