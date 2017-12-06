#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

void ChangeIndError(int t_i, double t_factor)
{
	// name of the file to be read
	std::stringstream ss_input;
	ss_input << "original/run00";
	ss_input << std::setw(2) << std::setfill('0') << t_i;
	ss_input << "/results.dat";
	std::ifstream input;
	input.open(ss_input.str().c_str());
	if(!input.is_open()) 
	{
		std::cerr << " Model file cannot be opened\n";
		return;
	}
	
	// open the file to which the result will stores
	std::ofstream output;
	std::stringstream ss_output;
	ss_output << "run00";
	ss_output << std::setw(2) << std::setfill('0') << t_i;
	ss_output << "/results.dat";
	output.open(ss_output.str().c_str());
	if(!output.is_open())
	{
		std::cerr << " Output file cannot be opened\n";
		return;
	}
	
	// read the input file line by line
	std::string buffer1, buffer2;
	double error;
	while(input >> buffer1 >> buffer2 >> error)
	{
		std::cout << buffer1 << "\n";
		error *= t_factor;
		output << buffer1 << " " << buffer2 << " " << error << "\n";
	}
	output.close();
	input.close();
	
}

int main()
{
	for(int i = 1; i < 50; ++i)
		ChangeIndError(1, 2.);
	return 0;
}
