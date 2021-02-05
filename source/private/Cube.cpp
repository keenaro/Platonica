#include "Cube.h"

bool Cube::CanSee() const
{
	return GetID() != CubeID::Air;
}

void Cube::SetFaceData(CubeFace faces)
{
	m_data -= (m_data & (CubeFace::All << 10));
	m_data += faces << 10;
}

void Cube::CullFace(CubeFace faceToCull)
{
	uint32_t facesToCull = GetFaceData() & faceToCull;

	if (facesToCull)
	{
		CubeFace faceData = GetFaceData();
		CubeFace face = CubeFace(faceData - facesToCull);
		SetFaceData(face);
	}
}