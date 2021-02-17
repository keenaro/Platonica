#pragma once
#include <deque>
#include <queue>
#include "Defs.h"
#include <glm/vec3.hpp>
#include <Thread>
#include <atomic>
#include <mutex>

class Chunk;

class AsyncChunkManager
{
public:
	AsyncChunkManager::~AsyncChunkManager();

public:
	void RequestTask(SharedPtr<class Chunk> chunk);
	void Start();
	void DoTask();
private:
	void DoWork();

private:
	std::queue<SharedPtr<class Chunk>> requestedTasks;
	std::thread workThread;
	std::atomic_bool shouldWork{ true };
	std::mutex requestingTask;
};

