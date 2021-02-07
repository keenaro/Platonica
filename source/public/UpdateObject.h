#pragma once
#include "ShaderHelper.h"
#include <vector>

class UpdateObject
{
public:
	UpdateObject(bool addToUpdateList = true);
	~UpdateObject();

	virtual void Update(float deltaTime) = 0;
	virtual bool ShouldUpdate() const { return true; };
};
