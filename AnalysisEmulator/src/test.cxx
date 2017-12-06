#include "ProcessPipe.h"
#include "test.h"
//#include "EmulatorOutputReader.h"
#include "EmulatorInterface.h"
#include "DataSelector.h"
#include "RunInputReader.h"
#include "RunMaster.h"
#include "EmulatorConstants.h"
#include "CSVReader.h"
#include "SettingSetter.h"
#include "Data.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>
#include <regex>
#include <iomanip>
#include <regex>

//double chi_square(double, double);
// return points linearly intrapolated between the 2 entered vectors
// fraction represent how close the point is to the first point
std::vector<double> intrapolate(const std::vector<double>& t_first, const std::vector<double>& t_second, double t_fraction)
{
	if(t_second.size() != t_first.size()) std::cerr << " interpolate vector size doesn't match\n";
	int dimension = t_first.size();
	
	std::vector<double> pos(dimension);
	for(int i = 0; i < dimension; ++i)
	{
		pos[i] = t_first[i] + t_fraction * (t_second[i] - t_first[i]);
		//std::cout << pos[i] << " " ;
	}
	//std::cout << "\n";
	
	return pos;
}

void check_emulator_error_bar(double t_scales, double t_nuggets,const std::string& t_path)
{
	RunMaster controller("/projects/hira/MADAI/Tsangc/test_distribution/e120", "e120/");
	controller.SetScales(t_scales);
	controller.SetNuggets(t_nuggets);
	controller.ExcludeRun(std::vector<int>{60});

	// files to be written
	int start_no = controller.GetStartNo();
	int no_run = controller.GetEndNo() - start_no + 1;
	int var_num = controller.GetVarFromRun(start_no).size();
	std::vector<std::ofstream> file(var_num);
	std::vector<std::ofstream> from_run(var_num);
	for(int j = 0; j < var_num; ++j)
	{
		std::string filename = t_path/*"./error_bar*/ + "/error_bar_for_" + std::to_string(j) + ".txt";
		file[j].open(filename.c_str());
		filename = t_path + "/original_for_" + std::to_string(j) + ".txt";
		from_run[j].open(filename.c_str());
	}
		
	// find equation of straight lines connecting parameters for different runs
	for(int i = start_no; i < no_run + start_no - 1; ++i)
	{	
		std::vector<double> first_point = controller.GetParFromRun(i);
		std::vector<double> second_point = controller.GetParFromRun(i + 1);
		for(double fraction = 0; fraction <= 0.99; fraction += 0.1)
		{
			std::vector<double> par = intrapolate(first_point, second_point, fraction);
			std::vector<Data> data = controller.RunEmulator(par);
			// get rid of nan error, set them to zero
			for(int var = 0; var < var_num; ++var)
			{
				if(std::isnan(data[var].err)) data[var].err = 0.;
				file[var] << (double) i + fraction << " " << data[var].val << " " << data[var].err << "\n";
			}
		}
		for(int var = 0; var < var_num; ++var)
		{
			std::vector<Data> data = controller.GetVarFromRun(i);
			from_run[var] << i << " " << data[var].val << " " << data[var].err << "\n";
		}
		
	}

	for(int j = 0; j < var_num; ++j)
	{
		file[j].close();
		from_run[j].close();
	}
}

void check_emulator(int t_run)
{
	RunMaster controller("/projects/hira/MADAI/Tsangc/test_distribution2/e120", "e120/");
	double scales = 1e-2;
	double nuggets = 1e-3;
	std::cout << "scales = " << scales << " nuggets = " << nuggets << "\n";
	controller.SetScales(scales);
	controller.SetNuggets(nuggets);
	controller.SetSettings("EMULATOR_TRAINING_ALGORITHM", "exhaustive_geometric_kfold_common");
	controller.SetSettings("EMULATOR_REGRESSION_ORDER", std::to_string(2));
	std::ofstream emulator_output;
	// stringstream for filename format
	std::stringstream ss;
	ss << std::setw(4) << std::setfill('0') << t_run;
	std::string filename = "./result/run" + ss.str();
	emulator_output.open(filename.c_str());
	std::cout << filename << "\n";
	controller.ExcludeRun(std::vector<int>{t_run});
	std::vector<Data> data = controller.RunEmulator(controller.GetParFromRun(t_run));
	for(int j = 0; j < data.size(); ++j)
		emulator_output << j << " " << data[j].val << " " << data[j].err << "\n";
	emulator_output.close();

	// original run
	std::ofstream original_output;
	filename = "./result/original00" + ss.str();
	original_output.open(filename.c_str());
	data = controller.GetVarFromRun(t_run);
	for(int j = 0; j < data.size(); ++j)
		original_output << j << " " << data[j].val << " " << data[j].err << "\n";
	original_output.close();
}
//int main()
void check_nuggets_effect()
{
	RunMaster controller("/projects/hira/MADAI/Tsangc/Ri35_Emulate", "Ri35/");//test_distribution/e120");
	controller.SetScales(0.000001);
	int no_run = controller.GetEndNo() - controller.GetStartNo() + 1;

	std::ofstream real;
	real.open("NoEmulator");
	for(int i = controller.GetStartNo(); i < no_run + controller.GetStartNo(); ++i)
	{
		std::vector<Data> data = controller.GetVarFromRun(i);
		real << data[0].val << " " << data[0].err << "\n";
	}
	for(double nuggets = 0.1; nuggets < 10; nuggets += 1.)
	{
		controller.SetNuggets(nuggets);
		std::regex floating("([0-9]+)\\.*([0-9]*+)");
		std::smatch match;
		std::regex_match(std::to_string(nuggets), match, floating);
		//std::cout << nuggets << " " << match[1].str() << " " << match[2].str() << "\n";
		std::string head = match[1].str(), feet = match[2].str();
		std::string filename = "nuggets" + head + "_" + feet;
		std::cout << filename << " " << head << " " << feet << "\n";
		std::ofstream file;
		file.open(filename.c_str());
		for(int i = controller.GetStartNo(); i < no_run + controller.GetStartNo(); ++i)
		{
			std::cout << " Handling nuggets " << nuggets << " with run " << i << "\r";
			controller.ExcludeRun(std::vector<int>{i});
			std::vector<Data> data = controller.RunEmulator(controller.GetParFromRun(i));
			file << data[0].val << " " << data[0].err << "\n";
		}
		file.close();
	}
}

std::vector<std::vector<double>> chi_square(const std::vector<double>& t_nuggets, const std::vector<double>& t_scales, const std::vector<int>& t_excluded)
{
	std::vector<std::vector<double>> answer(t_nuggets.size(), std::vector<double>(t_scales.size(), 0));
	double chi2tot = 0;
	double log_prob_sum = 0;
	RunMaster controller("/projects/hira/MADAI/Tsangc/test_distribution/e120", "e120/");
	controller.ExcludeRun(t_excluded);
	for(int i = 0; i < t_nuggets.size(); ++i)
	{
		for(int j = 0; j < t_scales.size(); ++j)
		{
			std::cout << " Processing chi square with nuggets = " 
				<< std::setw(5) << std::setfill('0') << t_nuggets[i] 
				<< " and scales = " 
				<< std::setw(5) << std::setfill('0') << t_scales[j] << "\r";
			std::cout.flush();
			controller.SetNuggets(t_nuggets[i]);
			controller.SetScales(t_scales[j]);
			for(int k = 0; k < t_excluded.size(); ++k)
			{
				std::vector<Data> data = controller.RunEmulator(controller.GetParFromRun(t_excluded[k]));
				double log_prob = 0;
				for(int u = 0; u < data.size(); ++u)
				{
					Data from_run = controller.GetVarFromRun(t_excluded[k], u);
					log_prob += -log(from_run.err) - 0.5*pow((from_run.val - data[u].val)/from_run.err, 2) - 0.5*log(2*M_PI);
				}
				answer[i][j] += log_prob;
			}
		}
	}
	std::cout << "\n";
	return answer;
}

void chi_square_plot()
{
	// plot settings
	double scales_min = 1e-3;
	double scales_max = 2.;
	bool scales_log = false;
	int scales_num = 30;
	double nuggets_min = 1e-6;
	double nuggets_max = 1.;
	bool nuggets_log = true;
	int nuggets_num = 30;
	// calculate interval size
	double scales_interval, nuggets_interval;
	if(!scales_log) scales_interval = (scales_max - scales_min)/(double) scales_num;
	else scales_interval = pow((scales_max/scales_min), 1./(double) scales_num);
	if(!nuggets_log) nuggets_interval = (nuggets_max - nuggets_min)/(double) nuggets_num;
        else nuggets_interval = pow((nuggets_max/nuggets_min), 1./(double) nuggets_num);

	// vectors storing all sampled scales and nuggets
	std::vector<double> nuggets(nuggets_num + 1);
	std::vector<double> scales(scales_num + 1);
	scales[0] = scales_min;
	for(int i = 1; i <= scales_num; ++i)
	{
		if(!scales_log) 
			scales[i] = scales[i - 1] + scales_interval;
		else 
			scales[i] = scales[i - 1] * scales_interval;
	}
	nuggets[0] = nuggets_min;
	for(int i = 1; i <= nuggets_num; ++i)
	{
		if(!nuggets_log) 
			nuggets[i] = nuggets[i - 1] + nuggets_interval;
		else 
			nuggets[i] = nuggets[i - 1] * nuggets_interval;
	}

	// list of excluded runs
	std::vector<std::vector<int>> excluded_list;
	std::vector<int> temp;
	int row_index = -1;
	int partition = 10;
	for(int i = 0; i < 49; ++i)
	{
		if( i%partition == 0)
		{
			excluded_list.push_back(temp);
			++row_index;
		}
		excluded_list[row_index].push_back(i + 1);
	}

	// vectors storing all the result
	std::vector<std::vector<std::vector<double>>> chi_square_list(excluded_list.size());
	for(int i = 0; i < excluded_list.size(); ++i)
		chi_square_list[i] = chi_square(nuggets, scales, excluded_list[i]);

	// files to be written
	std::ofstream output;
	output.open("output.dat");
	if(!output.is_open()) std::cerr << " Files cannot be opened \n";
	output << " nuggets \\ scales \n";
	output << "\t";
	for(int i = 0; i <= scales_num; ++i)
	{
		output << scales[i] << "\t";
	}
	output << "\n";
	// check various value of chi square
	for(int j = 0; j <= nuggets_num; ++j)
	{
		output << nuggets[j] << "\t";
		for(int i = 0; i <= scales_num; ++i)
		{
			double tot_chi = 0;
			for(int k = 0; k < excluded_list.size(); ++k)
				tot_chi += chi_square_list[k][j][i];
			output << tot_chi << "\t";
		}
		output << "\n";
	}
	output.close();
}

void view_result(double t_nuggets, double t_scales, const std::vector<double>& t_parameters)
{
	RunMaster controller("/projects/hira/MADAI/Tsangc/test_distribution/e120", "e120/");
	controller.SetScales(t_scales);
	controller.SetNuggets(t_nuggets);
	controller.ExcludeRun(std::vector<int>{60});

	std::vector<Data> data = controller.RunEmulator(t_parameters);
	// retrieve data from experiment
	RunInputReader reader("e120/");
	reader.LoadRunFile("/projects/hira/MADAI/Tsangc/test_distribution/e120/experimental_data/results.dat");
	int no_var = reader.GetVarNum();
	std::vector<double> exp_results(no_var);
	std::vector<double> exp_results_error(no_var);
	for(int i = 0; i < no_var; ++i)
	{
		exp_results[i] = reader.GetVar(i);
		exp_results_error[i] = reader.GetError(i);
	}

	// output to file
	std::ofstream emulator_file, experimental_file;
	emulator_file.open("./experimental_result_compare/emulator_result.dat");
	experimental_file.open("./experimental_result_compare/experimental_result.dat");
	if(!emulator_file.is_open()) std::cerr << "Output file for emulator cannot be opened\n";
	if(!experimental_file.is_open()) std::cerr << "Output file for experiment cannot be opened\n";

	// write emulator file
	for(int i = 0; i < no_var; ++i)
		emulator_file << i << " " << data[i].val << " " << data[i].err << "\n";

	// write experimental file
	for(int i = 0; i < no_var; ++i)
		experimental_file << i << " " << exp_results[i] << " " << exp_results_error[i] << "\n";

	emulator_file.close();
	experimental_file.close();
	
}
void RunInterface(int argc, char** argv)
{
	RunMaster controller("/projects/hira/MADAI/Tsangc/test_distribution/e120", "e120/");
	controller.ExcludeRun(std::vector<int>{60});
	controller.SetScales(0.839);
	controller.SetNuggets(0.35);
	std::vector<double> par(4);
	for(int i = 1; i < 5; ++i)
	{
		par[i - 1] = atof(argv[i]);
	}
	std::vector<Data> var = controller.RunEmulator(par, true);
	for(int i = 0; i < var.size(); ++i)
		std::cout << var[i].val << " ";
	std::cout << "\n";
	for(int i = 0; i < var.size(); ++i)
		std::cout << var[i].err << " ";
	std::cout << "\n";
}

void GenerateAllTrace()
{
	// all possible combinations of options
	std::vector<double> nuggets{0.001, 0.839};
	std::vector<double> scales{0.01, 0.35};

	RunMaster controller("/projects/hira/MADAI/Tsangc/test_distribution/e120", "e120/");
	controller.ExcludeRun(std::vector<int>(60));	

	// loop through all possible combination of settings
	for(int i = 0 ; i < nuggets.size() + 1; ++i)
		for(int mcmc_error = 0; mcmc_error < 2 ; ++mcmc_error)
			for(int pca_error = 0; pca_error < 2; ++pca_error)
			{
				std::cout << " Generating trace for :\n";
				if(i == nuggets.size()) 
				{
					controller.SetSettings("EMULATOR_TRAINING_ALGORITHM", "exhaustive_geometric_kfold_common");
					std::cout << " exhaustive_geometric_kfold_common\n";
				}
				else
				{
					controller.SetNuggets(nuggets[i]);
					controller.SetScales(scales[i]);
					std::cout << " Scales " << scales[i] << " Nuggets " << nuggets[i] << "\n";
				}
				if(mcmc_error == 0) 
				{
					controller.SetSettings("MCMC_USE_MODEL_ERROR", "1");
					std::cout << " mcmc use model error\n";
				}
				if(pca_error == 1) 
				{
					controller.SetSettings("PCA_USE_MODEL_ERROR", "1");
					std::cout << " pca use model error\n";
				}
				controller.GenerateTrace();
			}
}

int main(int argc, char** argv)
{
	//GenerateAllTrace();
	/*double scales = 0.839;
	double nuggets = 0.35;
	RunMaster controller("/projects/hira/MADAI/Tsangc/test_distribution/e120", "e120/");
	controller.SetScales(scales);
	controller.SetNuggets(nuggets);
	//controller.SetSettings("EMULATOR_TRAINING_ALGORITHM", "exhaustive_geometric_kfold_common");
	// leave one out, x-axis is the model value, y-axis is the emulator value
	std::ofstream model_vs_emulator;
	std::string filename = "model_vs_emulator_Nuggets_" + std::to_string(nuggets) + "Scales_" + std::to_string(scales) + ".dat";
	model_vs_emulator.open(filename.c_str());
	model_vs_emulator << "Model\tEmulator\tModel error\tEmulator error\tS0\tL\tmi\tfi\n";
	for(int i = controller.GetStartNo(); i <= controller.GetEndNo(); ++i)
	{
		std::cout << "Processing Run " << i << "\n";
		controller.ExcludeRun(std::vector<int>{i});
		std::vector<double> par = controller.GetParFromRun(i);
		std::vector<Data> var_emulator = controller.RunEmulator(par);
		std::vector<Data> var_model = controller.GetVarFromRun(i);
		for(int i = 0; i < var_model.size(); ++i)
			model_vs_emulator << var_model[i].val << "\t" << var_emulator[i].val << "\t" <<  var_model[i].err << "\t" << var_emulator[i].err << "\t" << par[0] << "\t" << par[1] << "\t" << par[2] << "\t" << par[3] << "\n";
	}*/
	/*for(int i = controller.GetStartNo(); i <= controller.GetEndNo(); ++i)
	{
		check_emulator(i);
	}*/
	//check_nuggets_effect();
	chi_square_plot();	
	//view_result(0.001, 0.01, std::vector<double>{31.1212, 57.1354, 0.601243, -0.0024773});
	//view_result(0.002, 2., std::vector<double>{27.089, 32.0051, 0.825975, 0.00299572});
	//view_result(0.35, 0.839, std::vector<double>{27.1918, 32.0229, 0.836986, -0.003482});
	//check_emulator_error_bar(1, 1, "./error_bar/nuggets1_scales1/");
	//check_emulator_error_bar(1, 0.1, "./error_bar/nuggets0.1_scales1/");
	//check_emulator_error_bar(1, 0.01, "./error_bar/nuggets0.01_scales1/");
	//check_emulator_error_bar(0.1, 0.01, "./error_bar/nuggets0.01_scales0.1/");
	//check_emulator_error_bar(0.01, 0.01, "./error_bar/nuggets0.01_scales0.01/");
	//check_likelihood_partition();
	/*std::ofstream out_csv;
	out_csv.open("/projects/hira/MADAI/Tsangc/new_distribution/e120.csv");
	CSVReader reader;
	reader.LoadFile("/projects/hira/MADAI/Tsangc/new_distribution/e120_scale1.5_nuggets10.csv");
	RunMaster controller("/projects/hira/MADAI/Tsangc/test_distribution/e120", "e120/");
	controller.SetNuggets(10);
	controller.SetScales(1.5);
	controller.ExcludeRun(std::vector<int>{60});
	std::vector<std::string> var_name = reader.GetVarName();
	out_csv << "\"" << var_name[0] << "\"";
	for(int i = 1; i < var_name.size(); ++i)
	{
		out_csv << ",\"" << var_name[i] << "\"";
	}
	out_csv << "\n";
	for(int i = 0; i < 10000; ++i)
	{
		std::vector<double> display = reader.GetVarVec(0,3,i);
		std::vector<Data> data = controller.RunEmulator(display);
		out_csv << display[0];
		for(int j = 1; j < display.size(); ++j)
			out_csv << "," << display[j];
		for(int j = 0; j < data.size(); ++j)
			out_csv << "," << data[j].val;
		out_csv << "\n";
	}
	out_csv.close();*/
	/*SettingSetter setter("/projects/hira/MADAI/Tsangc/AnalysisEmulator/template/settings.dat.temp", "/projects/hira/MADAI/Tsangc/test_distribution/e120");
	setter.Set("EMULATOR_SCALE", std::to_string(0.1));
	setter.ApplySetting();*/
	return 0;
}
