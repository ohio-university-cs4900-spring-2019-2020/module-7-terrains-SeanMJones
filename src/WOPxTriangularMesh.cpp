#include "WOPxTriangularMesh.h"
#include <iostream>

using namespace Aftr;


WOPxTriangularMesh* WOPxTriangularMesh::New(const VectorD& upperLeft, const VectorD& lowerRight, unsigned int granularity, const VectorD& offset, const VectorD& scale, std::string elevationData, int splits, float exageration, bool useColors) {
	WOPxTriangularMesh* grid = new WOPxTriangularMesh();
	grid->onCreate(upperLeft, lowerRight, granularity, offset, scale, elevationData, splits, exageration, useColors);
	return grid;
}

WOPxTriangularMesh::WOPxTriangularMesh() : IFace(this), WOGridECEFElevation() {
}

void WOPxTriangularMesh::onCreate(const VectorD& upperLeft, const VectorD& lowerRight, unsigned int granularity, const VectorD& offset, const VectorD& scale, std::string elevationData, int splits = 2, float exageration = 0, bool useColors = false) {
	WOGridECEFElevation::onCreate(upperLeft, lowerRight, 0, offset, scale, elevationData, splits, exageration, useColors);
}