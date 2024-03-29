#pragma once
#include <glm/vec3.hpp>
#include "CubeDefs.h"

enum CubeFace
{
	None = 0,

	Front = 1,
	Back = 1 << 1,
	Right = 1 << 2,
	Left = 1 << 3,
	Bottom = 1 << 4,
	Top = 1 << 5,

	All = Front + Back + Right + Left + Bottom + Top,
};

class Cube
{
public:
	Cube(CubeID id = Air) { SetID(id); };

	bool CanSee() const;
	bool IsSolid() const;

	void SetID(CubeID id) { m_data = (uint32_t(id) << 16) + (m_data & 63); };
	void SetFaceData(CubeFace faces);

	void CullFace(CubeFace faceToCull);

	CubeID GetID() const { return CubeID(m_data >> 16); };
	CubeFace GetFaceData() const { return CubeFace((m_data >> 10) & 63); };
	int GetNumberOfFaces() const;

	bool operator==(const Cube& rhs) const { return m_data == rhs.m_data; }
	bool operator!=(const Cube& rhs) const { return !(*this == rhs); }

private:
	uint32_t m_data;
};