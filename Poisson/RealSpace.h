#pragma once


#include <eigen/eigen>


#include "Vector3D.h"

namespace Poisson {

	// the axes need not be orthogonal, for now they will be

	class RealSpaceCell
	{
	public:
		RealSpaceCell(double dim1, double dim2, double dim3, unsigned int samples1, unsigned int samples2, unsigned int samples3);

		const Vector3D<double>& GetSize() const { return m_dim; }
		const Vector3D<int>& GetSamples() const { return m_samples; }

		double Volume() const { return volume; }
		int Samples() const { return samples; }
		double SampleVolume() const { return sampleVolume; }

	private:
		const Vector3D<double> m_dim;

		const Vector3D<int> m_samples;

		const double volume;
		const unsigned int samples;
		const double sampleVolume;

	public:
		Eigen::Matrix<Vector3D<int>, Eigen::Dynamic, 1> Indices;
		Eigen::Matrix<Vector3D<double>, Eigen::Dynamic, 1> SamplePoints;
	};

}
