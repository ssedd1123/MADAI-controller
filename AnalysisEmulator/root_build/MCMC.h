#ifndef MCMC_H
#define MCMC_H
#include <vector>
#include <functional>
#include <random>

typedef std::function<double(const std::vector<double>&)> Func;

class MCMC
{
public:
	MCMC(const Func t_func, const std::vector<double> t_init): 
		distribution_(t_func),
		rand_(0., 1),
		step_{t_init}
	{};

	void SetBound(const std::vector<double>& t_lower, const std::vector<double>& t_upper)
	{
		lower_bound_ = t_lower;
		upper_bound_ = t_upper;	
	};

	double Eval(const std::vector<double>& t_para)
	{
		return distribution_(t_para);
	};

	
	std::vector<double> NextStep()
	{
		while(true)
		{
			// generate proposed next step
			std::vector<double> new_step(step_.size());
	
			// check boundary, if either fall outside, we drop it
			for(unsigned int i = 0; i < new_step.size(); ++i)
			{
				if(new_step[i] > upper_bound_[i] || new_step[i] < lower_bound_[i])
					continue;
				new_step[i] = RandomNum(lower_bound_[i], upper_bound_[i]);
			}
	
			// calculate acceptance probability
			double prob = distribution_(new_step)/distribution_(step_);
			prob = (prob > 1)? 1 : prob;
	
			// generate random number and see if the next step is accepted
			double rand_num = rand_(generator_);
			if(rand_num < prob)
			{
				step_ = new_step;
				return step_;
			}
		}
	}
private:
	double RandomNum(double t_lower, double t_upper)
	{
		return (t_upper - t_lower) * rand_(generator_) + t_lower;
	};
	std::vector<double> step_;
	std::default_random_engine generator_;
	std::uniform_real_distribution<double> rand_;
	Func distribution_;

	std::vector<double> lower_bound_, upper_bound_;
};

#endif
