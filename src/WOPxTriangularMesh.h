#pragma once

#include "WOGridECEFElevation.h"
#include <vector>


namespace Aftr {
	class WO;

	class WOPxTriangularMesh : public WOGridECEFElevation {
	public:
		static WOPxTriangularMesh * New(const VectorD& upperLeft, const VectorD& lowerRight, unsigned int granularity, const VectorD& offset, const VectorD& scale, std::string elevationData, int splits = 2, float exageration = 0, bool useColors = false);
	protected:
		WOPxTriangularMesh();
		virtual void onCreate(const VectorD& upperLeft, const VectorD& lowerRight, unsigned int granularity, const VectorD& offset, const VectorD& scale, std::string elevationData, int splits, float exageration, bool useColors);
	};
}
