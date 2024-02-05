#include "stdafx.h"
#include "ReciprocalSpace.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Poisson {


	ReciprocalSpaceCell::ReciprocalSpaceCell(const RealSpaceCell& realSpaceCell)
		: volume(0)
	{
		Init(realSpaceCell);
	}

	void ReciprocalSpaceCell::Init(const RealSpaceCell& realSpaceCell)
	{
		const Vector3D<double>& dim = realSpaceCell.GetSize();
		const double twopi = 2. * M_PI;
		m_dim = Vector3D<double>(twopi / dim.X, twopi / dim.Y, twopi / dim.Z);
		volume = twopi * twopi * twopi / realSpaceCell.Volume();

		Indices.resize(realSpaceCell.Samples(), 1); 
		LatticeVectors.resize(realSpaceCell.Samples(), 1);
		LatticeVectorsSquaredMagnitude.resize(realSpaceCell.Samples(), 1);
		
		unsigned int i = 0;
		const Vector3D<int>& samples = realSpaceCell.GetSamples();

		for (int val1 = 0; val1 < samples.X; ++val1)
			for (int val2 = 0; val2 < samples.Y; ++val2)
				for (int val3 = 0; val3 < samples.Z; ++val3)
				{
					const Vector3D<int>& val = realSpaceCell.Indices(i);
					const Vector3D<int> offset(val.X > static_cast<int>(samples.X/2) ? samples.X : 0, val.Y > static_cast<int>(samples.Y/2) ? samples.Y : 0, val.Z > static_cast<int>(samples.Z/2) ? samples.Z : 0);
					const Vector3D<int> index(val - offset);

					Indices(i) =  index;
					
					Vector3D<double> latticeVector(index.X / dim.X, index.Y / dim.Y, index.Z / dim.Z);
					
					LatticeVectors(i) = 2. * M_PI * latticeVector;

					LatticeVectorsSquaredMagnitude(i) = LatticeVectors(i) * LatticeVectors(i);
					++i;
				}
	}


}
