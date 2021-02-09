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

int Cube::GetNumberOfFaces() const 
{
	const CubeFace faces = GetFaceData();
	return bool(faces & CubeFace::Front) + bool(faces & CubeFace::Back) + bool(faces & CubeFace::Right) + bool(faces & CubeFace::Left) + bool(faces & CubeFace::Bottom) + bool(faces & CubeFace::Top);
}