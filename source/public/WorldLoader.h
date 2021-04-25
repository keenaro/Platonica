#pragma once
#include "UpdateObject.h"
#include "Defs.h"

class WorldLoader : public UpdateObject
{
public:
	WorldLoader();
	void Update(float deltaTime) override;

private:
	void UpdateGUI();
	void CreateOrLoadWorld(const String& worldName, int seed = 0, int regionLength = 8);
	void ConnectToWorld(const char* hostname, int port);
private:
	std::vector<String> worldNames;

};