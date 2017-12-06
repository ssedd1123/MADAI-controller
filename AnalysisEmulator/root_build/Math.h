#ifndef MATH_H
#define MATH_H
#include <functional>
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <cmath>

namespace Constants
{
	const double ns = 0.16; // saturation density is 0.16 fm^-3
	const double phi = (1. + sqrt(5.))/2.; // golden ratio
        const double G = 1;
        const double c = 1;
        const double m = 1;
	const double exp = std::exp(1);
        //const double e0 = 56.39; // MeV fm-3
        const double es = 150; // energy density at ns, MeV fm-3
        const double y2mass = 47.9934;
	const double radius_factor = 1.4771;
	const std::vector<double> n{1/2.7, 1.85, 3.74, 7.4};
        const std::vector<double> MassLimit{1.97, 2.1, 2.3, 2.5};
	const double mp = 938.28; // mass of proton in MeV / c2
	const double mn = 939.57; // mass of neutron in MeV / c2
	const double me = 0.511; // mass of electron in MeV / c2
	const double mmu = 105.658; // mass of muon in MeV / c2
	const double hbarc = 197.32697;

};

typedef std::function<double(double)> Function1D;

namespace Math
{
	/* s,t: endpoints of an interval where we search
	e: half of upper bound for relative error
	m: maximal number of iterations */
	template<class Func>
	double FalsiMethod(const Func& f, double s, double t, double e)
	{
		int m = 20;
		double r,fr;
		int n, side=0;
		/* starting values at endpoints of interval */
		double fs = f(s);
		double ft = f(t);
	
		for (n = 0; n < m; n++)
		{
		
			 r = (fs*t - ft*s) / (fs - ft);
			 if (fabs(t-s) < e*fabs(t+s)) break;
			 fr = f(r);

			 if (fr * ft > 0)
			 {
				/* fr and ft have same sign, copy r to t */
				t = r; ft = fr;
				if (side==-1) fs /= 2;
				side = -1;
			 }
			 else if (fs * fr > 0)
			 {
				/* fr and fs have same sign, copy r to s */
				s = r;  fs = fr;
				if (side==+1) ft /= 2;
				side = +1;
			 }
			 else
			 {
				/* fr * f_ very small (looks like zero) */
				break;
			 } 
		 }
		 return r;
	}
	
	template<class Func>
	double Brent_Method(const Func& t_func, double a, double b, double t_epsilon)
	{
		double fa = t_func(a);
		double fb = t_func(b);
		if(fa * fb > 0)
		{
			// automatic finder from range
			double x_max = (a > b)? a : b;
			double x_min = (a < b)? a : b;
			double x_step = (x_max - x_min) / 20;
			for(double x = x_min; x < x_max; x += x_step)
				if(t_func(x) * fa < 0) return Brent_Method(t_func, a, x, t_epsilon);


			std::cerr << " No opposite sign found in range. Will abort\n";
			return std::numeric_limits<double>::quiet_NaN();
		}

		if( fabs(fa) < fabs(fb) ) std::swap(a, b);
	
		double c = a;
		double fc = t_func(c);
		double s = b;
		double d = 0;
		double fs = t_func(s);
		bool flag = true;
		while( fabs(fs) > t_epsilon )
		{
			if( fabs(fa - fc) < t_epsilon && fabs(fb - fc) < t_epsilon )
				s = a*fb*fc/((fa - fb)*(fa - fc)) + b*fa*fc/((fb - fa)*(fb - fc)) + c*fa*fb/((fc - fa)*(fc - fb));
			else s = b - fb*(b - a)/(fb - fa);
		
			if( 	(s < (3.*a + b)/4. && s > b) ||
				(flag && fabs(s - b) > fabs(b - c)/2.) ||
				(!flag && fabs(s - b) > fabs( c - d)/2.) ||
				(flag && fabs(b - c) < t_epsilon) ||
				(!flag && fabs(c - d) < t_epsilon))
			{
				s = 0.5*(a + b);
				flag = true;
			} else flag = false;

			fs = t_func(s);
			d = c;
			c = b;
			fa = t_func(a); fb = t_func(b); fc = t_func(c);
			if(fa*fs < 0) b = s;
			else a = s;
			
			if(fabs(fa) < fabs(fb)) std::swap(a, b);
		}

		return s;
	}

	template<class Func>
	double diff(const Func& t_func, double t_x, double t_dx)
	{
		double forward = t_func(t_x + t_dx);
		double backward = t_func(t_x - t_dx);
		return (forward - backward) / (2*t_dx);
	};

	template<class Func>
	double diff2(const Func& t_func, double t_x, double t_dx)
	{
		double forward = t_func(t_x + t_dx);
		double backward = t_func(t_x - t_dx);
		double center = t_func(t_x);
		return (forward - 2*center + backward)/(t_dx * t_dx);
	}

	template<class Func>
	double diff_5PtStencil(const Func& t_func, double t_x, double t_dx)
	{
		return (- t_func( t_x +2*t_dx) + 8*t_func(t_x + t_dx) - 
			8*t_func(t_x - t_dx) + t_func(t_x -2*t_dx)) / (12*t_dx);
	};

	template<class Func>
	double Bisection_Method(const Func& t_func, double t_x1, double t_x2, double t_epsilon)
	{		
		double func1 = t_func(t_x1);
		double func2 = t_func(t_x2);
		if(func1 * func2 > 0)
		{
			// automatic finder from range
			double x_max = (t_x1 > t_x2)? t_x1 : t_x2;
			double x_min = (t_x1 < t_x2)? t_x1 : t_x2;
			double x_step = (x_max - x_min) / 20;
			for(double x = x_min; x < x_max; x += x_step)
				if(t_func(x) * func1 < 0) return Bisection_Method(t_func, t_x1, x, t_epsilon);


			std::cerr << " No opposite sign found in range. Will abort\n";
			return std::numeric_limits<double>::quiet_NaN();
		}


		double mid_pt, result;
		int counter = 0, counter_max = 40;
		do
		{
			mid_pt = 0.5*(t_x1 + t_x2);
			result = t_func(mid_pt);
			if(t_func(mid_pt) * func1 > 0) t_x1 = mid_pt;
			else t_x2 = mid_pt;
			++counter;
			if(counter > counter_max)
			{
				std::cerr << " Bisection reaches its limit. Will abort\n";
				return std::numeric_limits<double>::quiet_NaN();
			}
		}
		while(fabs(result) > t_epsilon);
		return mid_pt;
	}

	template<class Func>
	double Newtons_Method(const Func& t_func, double t_x, double t_dx, double t_epsilon)
	{	
		const int call_limit = 20;
		int counter;	
		double result = fabs(t_func(t_x));
		for(counter = 1;  result > t_epsilon && counter <= call_limit; t_x -= t_func(t_x)/diff(t_func, t_x, t_dx), ++counter) 
		{
			if(std::isnan(result)) 
			{
				std::cerr << " Newton's method ran outside valid domain\n";
				return std::numeric_limits<double>::quiet_NaN();
			}
			result = fabs(t_func(t_x));
		}
		if(counter > call_limit) 
		{
			std::cerr << " Newton's method fails to find roots: call limit reached\n";
			return std::numeric_limits<double>::quiet_NaN();
		}
		return t_x;
	};

	template<class Func>
	double Solver(const Func& t_func, double t_x, double t_dx, double t_epsilon, double t_min = 1, double t_max = 40)
	{
		double ans = Newtons_Method(t_func, t_x, t_dx, t_epsilon);
		if(std::isnan(ans)) ans = Brent_Method(t_func, t_min, t_max, t_epsilon);
		return ans;
	}

	template<class Func>
	double NewMaximizer(const Func& t_func, double t_a, double t_b, double t_epsilon)
	{
		double a = t_a;
		double search_steps = (t_b - t_a) / 20.;
		double b = a + search_steps;
		double c = b - (b - a)/Constants::phi;
		double d = a + (b - a)/Constants::phi;
		double fa = t_func(a);
		double fb = t_func(b);
		double fc = t_func(c);
		double fd = t_func(d);

		// check if maximum really lies in the boundary
		if(!(fa < fc && fb < fc))
		{
			for(;b <= t_b; b += search_steps)
			{
				c = b - (b - a)/Constants::phi;
				d = a + (b - a)/Constants::phi;
				fa = t_func(a);
				fb = t_func(b);
				fc = t_func(c);
				fd = t_func(d);
				if((fa < fc && fb < fc)) break;
			}
			//if(b > t_b) std::cerr << "The function may have no or more than 1 max. point\n";
		}

		while(fabs(a - b) > t_epsilon*(fabs(c) + fabs(d))) 
		{
			if(fc > fd)
			{
				b = d; fb = fd;
				d = c; fd = fc;
				c = b + (a - b)/Constants::phi;
				fc = t_func(c);
			}else
			{
				a = c; fa = fc;
				c = d; fc = fd;
				d = a + (b - a)/Constants::phi;
				fd = t_func(d);
			}
		}

		return 0.5*(a + b);
		// find max

	}

	template<class Func>
	double Maximize(const Func& t_func, double t_x, double t_dx, double t_epsilon, double t_min = 1, double t_max = 40)
	{
	/*	TCanvas *c1 = (TCanvas*) gROOT->FindObject("max");
		if(!c1) c1 = new TCanvas("max");
		c1->cd();
		TF1 tf1("tf1", [&](double *x, double *p){return t_func(x[0]);}, t_min, t_max, 0);
		tf1.Draw();
		gPad->WaitPrimitive();*/
		double ans = NewMaximizer(t_func, t_min, t_max, t_epsilon);
		return ans;
	}

	// for multidimensional function, it must accept vector and output double
	template<class Func>
	std::vector<double> GradDescent(const Func& t_func, std::vector<double> t_start, double t_dx = 1e-3, double t_epsilon = 1e-5, double t_step_size = 0.1)
	{
		int var_num = t_start.size();
		std::vector<double> min_point(t_start);
		std::vector<double> prev_min_point(var_num, 0);
	
		std::vector<std::function<double(const std::vector<double>&)>> partial_diff;
		for(int i = 0; i < var_num; ++i)
			partial_diff.push_back([&, i](const std::vector<double>& point)
				{
					std::vector<double> forward(point);
					forward[i] += t_dx;
					std::vector<double> backward(point);
					backward[i] -= t_dx;
					return (t_func(forward) - t_func(backward))/ (2*t_dx);
				});

		for(int counter = 0; counter < 100; ++counter)
		{
			for(unsigned int i = 0; i < var_num; ++i)
				cout << min_point[i] << " ";
			cout << endl;

			//vector pointing to the direction of gradient
			std::vector<double> dir(var_num);
			double size2 = 0;
			for(unsigned int i = 0; i < var_num; ++i)
			{
				dir[i] = partial_diff[i](min_point);
				min_point[i] += t_step_size*dir[i];
				size2 += dir[i]*dir[i];
			}
			cout << sqrt(size2) << endl;
			if(sqrt(size2) < t_epsilon)
				return min_point;

			/*auto search_in_dir = [&](double x)->double
			{
				std::vector<double> point(min_point);
				for(int i = 0; i < var_num; ++i)
					point[i] += x*dir[i];
				return -t_func(point);
			};

			double factor = Math::Maximize(search_in_dir, 1, t_dx, 1e-4, -2., 0);
				
			for(int i = 0; i < var_num; ++i)
				min_point[i] += factor * dir[i];*/
		}

		std::cerr << " Gradient descent fails to find minimum\n";
		return std::vector<double>{};

	};

	template<class Func>
	std::vector<double> GradDescentStep(const Func& t_func, std::vector<double> t_start, double t_dx = 1e-3, double t_step_size = 0.1)
	{
		int var_num = t_start.size();
		std::vector<double> min_point(t_start);
		std::vector<double> prev_min_point(var_num, 0);
	
		std::vector<std::function<double(const std::vector<double>&)>> partial_diff;
		for(int i = 0; i < var_num; ++i)
			partial_diff.push_back([&, i](const std::vector<double>& point)
				{
					std::vector<double> forward(point);
					forward[i] += t_dx;
					std::vector<double> backward(point);
					backward[i] -= t_dx;
					return (t_func(forward) - t_func(backward))/ (2*t_dx);
				});


		//vector pointing to the direction of gradient
		std::vector<double> dir(var_num);
		double size2 = 0;
		for(unsigned int i = 0; i < var_num; ++i)
		{
			dir[i] = partial_diff[i](min_point);
			min_point[i] += t_step_size*dir[i];
			size2 += dir[i]*dir[i];
		}
		return min_point;

	};

	// for multidimensional function, it must accept vector and output double
	template<class Func>
	std::vector<double> ConjGrad(const Func& t_func, std::vector<double> t_start, double t_dx = 1e-4, double t_epsilon = 1e-5)
	{
		TCanvas *c1 = new TCanvas("c1");
		TF2 tf2("tf2", [&](double *x, double *p){return t_func(std::vector<double>{x[0], x[1]});}, 0, 1.5, 0, 1.5, 0);
		tf2.Draw("colz");
	
		int var_num = t_start.size();
		std::vector<double> min_point(t_start);
		std::vector<double> prev_min_point(var_num, 0);
	
		std::vector<std::function<double(const std::vector<double>&)>> partial_diff;
		for(int i = 0; i < var_num; ++i)
			partial_diff.push_back([&, i](const std::vector<double>& point)
				{
					std::vector<double> forward(point);
					forward[i] += t_dx;
					std::vector<double> backward(point);
					backward[i] -= t_dx;
					return (t_func(forward) - t_func(backward))/ (2*t_dx);
				});

		std::vector<double> dir(var_num);
		std::vector<double> prev_dir(var_num,0);
		std::vector<double> conj_dir(var_num);
		std::vector<double> prev_conj_dir(var_num, 0);

		c1->cd();
		TGraph graph;
		graph.SetPoint(graph.GetN(), t_start[0], t_start[1]);
		graph.Draw("PL same");

		double prev_size2 = 1;
		for(int counter = 0; counter < 100; ++counter)
		{
			double conj_size2 = 0;
			double size2 = 0;
			double beta = 0;
			cout << " dir ";
			for(unsigned int i = 0; i < var_num; ++i)
			{
				dir[i] = - partial_diff[i](min_point);
				size2 += dir[i]*dir[i];
				cout << dir[i] << " ";

			}
			cout << "\n";

			if(sqrt(size2) < t_epsilon)
			{
				gPad->WaitPrimitive();
				return min_point;
			}

			// Fletcher-Reeves:
			beta = size2/prev_size2;
			prev_size2 = size2;
			for(unsigned int i = 0; i < var_num; ++i)
			{
				conj_dir[i] = dir[i] + beta * prev_conj_dir[i];
				prev_conj_dir[i] = conj_dir[i];
				conj_size2 += conj_dir[i]*conj_dir[i];
			}

			// restric unit vector
//			for(unsigned int i = 0; i < var_num; ++i)
//				conj_dir[i] /= conj_size2;

			auto search_in_dir = [&](double x)->double
			{
				std::vector<double> point(min_point);
				for(int i = 0; i < var_num; ++i)
					point[i] += x*conj_dir[i];
				return -t_func(point);
			};

			double factor = Math::Maximize(search_in_dir, 1, t_dx, 1e-6, 0., 1.);
			
			//cout << " factor " << factor << "\n";	
			for(int i = 0; i < var_num; ++i)
				min_point[i] += factor * conj_dir[i];

			graph.SetPoint(graph.GetN(), conj_dir[0], conj_dir[1]);
			c1->cd();
			gPad->Update();
			gPad->Modified();
			gPad->WaitPrimitive();
		}

		std::cerr << " Gradient descent fails to find minimum\n";
		gPad->WaitPrimitive();
		return std::vector<double>{};

	};

};

#endif
