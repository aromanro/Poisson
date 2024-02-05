#pragma once

#include "RealSpace.h"

namespace Poisson {


	class ReciprocalSpaceCell
	{
	public:
		ReciprocalSpaceCell(const RealSpaceCell& realSpaceCell);

		void Init(const RealSpaceCell& realSpaceCell);

		const Vector3D<double>& GetSize() { return m_dim; }

		double Volume() const { return volume; }

		Eigen::Matrix<Vector3D<int>, Eigen::Dynamic, 1> Indices;
		Eigen::Matrix<Vector3D<double>, Eigen::Dynamic, 1> LatticeVectors;
		Eigen::Matrix<double, Eigen::Dynamic, 1> LatticeVectorsSquaredMagnitude;

	private:
		Vector3D<double> m_dim;

		double volume;
	};

}