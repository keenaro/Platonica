#pragma once
#include <deque>
#include <queue>
#include "Defs.h"
#include <glm/vec3.hpp>
#include <Thread>
#include <atomic>
#include <mutex>

class Chunk;

struct ChunkTask {
public:
	ChunkTask(glm::ivec3& inRegionPosition, glm::ivec3& inChunkPosition) : regionPosition(inRegionPosition), chunkPosition(inChunkPosition) {};

	glm::ivec3 regionPosition;
	glm::ivec3 chunkPosition;
	SharedPtr<Chunk> chunk;

	bool operator==(const ChunkTask& rhs) const
	{
		return chunkPosition == rhs.chunkPosition && regionPosition == rhs.regionPosition;
	}
};

class AsyncChunkManager
{
public:
	AsyncChunkManager::~AsyncChunkManager();

public:
	void MergeCompletedTasks(class World& world, bool mergeAll = false);
	void RequestTask(glm::ivec3& regionPosition, glm::ivec3& chunkPosition);
	void Start();
private:
	void DoWork();

private:
	std::queue<ChunkTask> requestedTasks;
	std::queue<ChunkTask> completedTasks;
	std::thread workThread;
	std::atomic_bool shouldWork{ true };
	std::mutex requestingTask;
	std::mutex completingTask;
};

