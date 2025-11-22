#pragma once


#include <complex>

#undef min
#undef max
#include <eigen/eigen>

#include <vector>


#include "Vector3D.h"

#include "FFT.h"

#include "Numerics.h"
#include "RealSpace.h"
#include "ReciprocalSpace.h"

namespace Poisson {

	class GaussianChargeDistribution
	{
	public:
		GaussianChargeDistribution(unsigned int z = 1);

		unsigned int Z;

		Vector3D<double> position; // relative to the center of the cell
	};


	class Charges
	{
	public:
		void ComputeStructureFactorAndChargeDensity(Fourier::FFT& fftSolver, Poisson::RealSpaceCell& realSpaceCell, Poisson::ReciprocalSpaceCell& reciprocalCell, double sigma = 0.25)
		{
			const int size = realSpaceCell.Samples();
			
			// structure factor
			StructureFactor = Eigen::VectorXcd::Zero(size);

			for (int i = 0; i < size; ++i)
				for (const auto& charge : charges)
					StructureFactor(i) += std::complex<double>(charge.Z, 0) * std::exp(std::complex<double>(0, -1) * (reciprocalCell.LatticeVectors(i) * charge.position));


			// charge density

			Eigen::VectorXcd g(size);


			const Vector3D<double> center = realSpaceCell.GetSize() / 2.;
	
			for (int i = 0; i < size; ++i)
			{
				const Vector3D<double> distanceVector(realSpaceCell.SamplePoints(i) - center);

				g(i) = Poisson::Numerics::Gaussian3D(distanceVector.Length(), sigma);
			}

			TRACE(L"Gaussian normalization check: %f\n", g.sum().real()*realSpaceCell.SampleVolume());

			rg.resize(size);

			fftSolver.fwd(g.data(), rg.data(), realSpaceCell.GetSamples().X, realSpaceCell.GetSamples().Y, realSpaceCell.GetSamples().Z);

			for (int i = 0; i < size; ++i)
			{
				rg(i) *= StructureFactor(i);
				rg(i) /= size;
			}

			ChargeDensity.resize(size);
			fftSolver.inv(rg.data(), ChargeDensity.data(), realSpaceCell.GetSamples().X, realSpaceCell.GetSamples().Y, realSpaceCell.GetSamples().Z);
		}

		std::vector<GaussianChargeDistribution> charges;

		Eigen::VectorXcd StructureFactor;
		Eigen::VectorXcd ChargeDensity;
		Eigen::VectorXcd rg;
	};


}