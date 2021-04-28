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
	void DeleteWorld(std::list<String>::const_iterator& world);
	void ConnectToWorld(const char* hostname, int port);
private:
	std::list<String> worldNames;

};