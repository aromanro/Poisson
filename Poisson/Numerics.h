#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

namespace Poisson {

	class Numerics
	{
	public:
		Numerics();
		~Numerics();

		static double Gaussian3D(double distance, double sigma) 
		{ 
			const double sigma2 = sigma * sigma;
			const double sq = sqrt(2. * M_PI * sigma2);
			const double sq3 = sq * sq * sq;

			return exp(-distance*distance / (2.*sigma2)) / sq3; 
		}
	};


}
