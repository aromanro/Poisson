#pragma once

#include <complex>
#include <eigen/eigen>


#include "FFT.h"
#include "Numerics.h"
#include "RealSpace.h"
#include "ReciprocalSpace.h"
#include "GaussianChargeDistribution.h"

namespace Poisson {

	class PoissonSolver
	{
	public:
		PoissonSolver();
		~PoissonSolver();

		static inline Eigen::VectorXcd SolveToReciprocalSpace(/*Fourier::FFT& fftSolver,*/ const Poisson::RealSpaceCell& realSpaceCell, Poisson::ReciprocalSpaceCell& reciprocalCell, Charges& charges)
		{
			//transform charge density from real space to reciprocal space:
			//Eigen::VectorXcd fieldReciprocal(realSpaceCell.Samples());
			//fftSolver.fwd(charges.ChargeDensity.data(), fieldReciprocal.data(), realSpaceCell.GetSamples().X, realSpaceCell.GetSamples().Y, realSpaceCell.GetSamples().Z);

			//or use the faster method, since we already have it in 'rg': 			
			Eigen::VectorXcd fieldReciprocal = realSpaceCell.Samples() * charges.rg;

			fieldReciprocal(0) = 0;
			for (int i = 1; i < realSpaceCell.Samples(); ++i)
			{
				// inverse Laplace operator
				fieldReciprocal(i) *= 4. * M_PI / realSpaceCell.Samples() / reciprocalCell.LatticeVectorsSquaredMagnitude(i);
			}

			return fieldReciprocal;
		}

		static inline Eigen::VectorXcd SolveToRealSpace(Fourier::FFT& fftSolver, Poisson::RealSpaceCell& realSpaceCell, Poisson::ReciprocalSpaceCell& reciprocalCell, Charges &charges)
		{
			Eigen::VectorXcd fieldReciprocal = SolveToReciprocalSpace(/*fftSolver,*/ realSpaceCell, reciprocalCell, charges);

			Eigen::VectorXcd field(realSpaceCell.Samples());

			fftSolver.inv(fieldReciprocal.data(), field.data(), realSpaceCell.GetSamples().X, realSpaceCell.GetSamples().Y, realSpaceCell.GetSamples().Z);

			return field;
		}
	};
}