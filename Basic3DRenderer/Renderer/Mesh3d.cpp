#include "pch.h"
#include "Mesh3d.h"
#include <fstream>
#include <sstream>

bool Mesh3d::LoadFromObjFile(std::string fileName)
{
	std::ifstream f(fileName);
	if (!f.is_open())
		return false;

	std::vector<Vector3d> verts;

	while (!f.eof())
	{
		char line[128];
		f.getline(line, 128);

		std::stringstream s;
		s << line;

		char junk;

		if (line[0] == 'v')
		{
			Vector3d v;
			s >> junk >> v.x >> v.y >> v.z;
			verts.push_back(v);
		}

		if (line[0] == 'f')
		{
			int f[3];
			s >> junk >> f[0] >> f[1] >> f[2];
			m_Tris.push_back(Triangle3d{ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
		}


	}

	return true;
}
