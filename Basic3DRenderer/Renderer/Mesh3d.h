#pragma once
#include "Triangle3d.h"
#include <vector>
#include <string>


struct Mesh3d
{
	std::vector<Triangle3d> m_Tris;

	bool LoadFromObjFile(std::string fileName);
};

