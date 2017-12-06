#include "EmulatorInterface.h"

//typedef std::chrono::high_resolution_clock Clock;

EmulatorInterface::~EmulatorInterface()
{
	if(pipe_) delete pipe_;
}

void EmulatorInterface::TrainEmulator(bool t_pca)
{
	if(path_.empty()) throw std::runtime_error("TrainEmulator Path not set correctly");
	// create the apporporate setting files from template
	std::string command; 
	//system(command.c_str());
	//auto t1 = Clock::now();
	command = "madai_pca_decompose " + path_;
	if(t_pca) system(command.c_str());
	//auto t2 = Clock::now();
	//std::cout << "PCA decompose: " 
        //      << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
        //      << " milliseconds" << std::endl;
	command = "madai_train_emulator " + path_;
	system(command.c_str());
	//t2 = Clock::now();
	//std::cout << "train: " 
        //      << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
        //      << " milliseconds" << std::endl;

	// must restart the process once the emulator is newly trained
	if(pipe_) 
	{
		pipe_->Input("STOP");
		delete pipe_;
	}
	command = "madai_emulate " +  path_;
	pipe_ = new ProcessPipe(command);
	//t2 = Clock::now();
	//std::cout << "start new pipe: " 
        //      << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
        //      << " milliseconds" << std::endl;
	// get rid of the header
	/*std::string answer;
	for(;(answer = pipe_->GetOutput()) != "END_OF_HEADER\n";)
		if(answer == "OUTPUTS\n")
			var_num_ = atof(pipe_->GetOutput().c_str());
	*/
	/*for(;pipe_->GetOutput() != "OUTPUTS\n";){}
	var_num_ = atof(pipe_->GetOutput().c_str());

	for(;pipe_->GetOutput() != "END_OF_HEADER\n";){}
*/

	pipe_->ReadUntilOutputIs("OUTPUTS\n");
	var_num_ = atof(pipe_->GetOutput().c_str());
	pipe_->ReadUntilOutputIs("END_OF_HEADER\n");
	//t2 = Clock::now();
	//std::cout << "count var: " 
        //      << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
        //      << " milliseconds" << std::endl;
}

void EmulatorInterface::RunEmulator(const std::vector<double>& t_parameters)
{
	std::string input = std::to_string(t_parameters[0]) + " ";
	for(unsigned int i = 1; i < t_parameters.size(); ++i)
		input = input + std::to_string(t_parameters[i]) + " ";
	pipe_->Input(input);
	if(var_num_ == 0) throw std::runtime_error("RunEmulator variable numbers is 0. Please train the emulator first");
	//int var_num_ = 33;
	var_.resize(var_num_);
	cov_.resize(var_num_);
	for(int i = 0; i < var_num_; ++i)
	{
		var_[i] = atof(pipe_->GetOutput().c_str());
	}
	// read the covariance matrix
	for(int i = 0; i < var_num_; ++i)
	{
		cov_[i].resize(var_num_);
		for(int j = i; j < var_num_; ++j)
		{
			cov_[i][j] = atof(pipe_->GetOutput().c_str());
		}
	}
}

void EmulatorInterface::SetWorkingDir(std::string t_path) 
{
	// path must not end in slash "/" 
	// check and see if we need to delete the slash
	std::regex path_format("(.*)\\/\\s*\\Z");
	std::smatch match;
	if(std::regex_match(t_path, match, path_format))
		path_ = match.str(1);
	else 
		path_ = t_path;
}

int EmulatorInterface::GetVarNum()
{
	return var_num_;
}

double EmulatorInterface::GetVar(int t_i)
{
	return var_[t_i];
}

double EmulatorInterface::GetCovMatrix(int t_i, int t_j)
{
	if(t_i > t_j) 
	{
		int temp = t_j;
		t_j = t_i;
		t_i = temp;
	}
	return cov_[t_i][t_j];
}

void EmulatorInterface::GetVarName(std::vector<std::string>& t_name)
{
	//_reader.GetVarName(t_name);
}
