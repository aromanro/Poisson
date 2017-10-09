#pragma once

#include <complex>
#include <eigen/eigen>


#include "FFT.h"
#include "Numerics.h"
#include "RealSpace.h"
#include "ReciprocalSpace.h"

namespace Poisson {

	class PoissonSolver
	{
	public:
		PoissonSolver();
		~PoissonSolver();

		static inline Eigen::VectorXcd SolveToReciprocalSpace(Fourier::FFT& fftSolver, Poisson::RealSpaceCell& realSpaceCell, Poisson::ReciprocalSpaceCell& reciprocalCell, Eigen::VectorXcd &chargeDensity)		
		{
			Eigen::VectorXcd fieldReciprocal(realSpaceCell.Samples());

			fftSolver.fwd(chargeDensity.data(), fieldReciprocal.data(), realSpaceCell.GetSamples().X, realSpaceCell.GetSamples().Y, realSpaceCell.GetSamples().Z);

			fieldReciprocal(0) = 0;
			for (int i = 1; i < realSpaceCell.Samples(); ++i)
			{
				// inverse Laplace operator
				fieldReciprocal(i) *= 4. * M_PI / realSpaceCell.Samples() / reciprocalCell.LatticeVectorsSquaredMagnitude(i);
			}

			return fieldReciprocal;
		}

		static inline Eigen::VectorXcd SolveToRealSpace(Fourier::FFT& fftSolver, Poisson::RealSpaceCell& realSpaceCell, Poisson::ReciprocalSpaceCell& reciprocalCell, Eigen::VectorXcd &chargeDensity)
		{
			Eigen::VectorXcd fieldReciprocal = SolveToReciprocalSpace(fftSolver, realSpaceCell, reciprocalCell, chargeDensity);

			Eigen::VectorXcd field(realSpaceCell.Samples());

			fftSolver.inv(fieldReciprocal.data(),field.data(),realSpaceCell.GetSamples().X, realSpaceCell.GetSamples().Y, realSpaceCell.GetSamples().Z);

			return field;
		}

	};

}