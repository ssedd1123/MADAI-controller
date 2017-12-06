
#ifndef TRACEANALYZER_H
#define TRACEANALYZER_H
#include "TH1.h"
#include "ProcessPipe.h"

class TraceAnalyzer
{
public:
	TraceAnalyzer(){};
	
	// name of the file, directory of the path, downscale ratio
	void AnalyzeTrace(const std::string& t_csv, const std::string& t_path, unsigned int t_downscale = 1);
	void GetParName(std::vector<std::string>& t_vec) { t_vec = name_;};
	void GetParMean(std::vector<double>& t_vec) { t_vec = mean_;};
	void GetParBest(std::vector<double>& t_vec) { t_vec = best_;};
	void GetParBestLikelihood(std::vector<double>& t_vec) { t_vec = best_likelihood_;};
	void GetParSD(std::vector<double>& t_vec) { t_vec = std_dev_;};
	void GetParScaledDev(std::vector<double>& t_vec) { t_vec = scaled_dev_;};
private:
	std::vector<double> mean_, std_dev_, scaled_dev_, best_, best_likelihood_;
	std::vector<std::string> name_;
};

#endif
