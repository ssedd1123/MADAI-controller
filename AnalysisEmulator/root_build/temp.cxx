#include <eigen3/Eigen/Dense>
#include <iostream>

void temp()
{
	Eigen::MatrixXd test = Eigen::MatrixXd::Random(33,33);
	std::cout << test.inverse() << std::endl;
}
