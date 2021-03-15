#pragma once
#include <deque>
#include <queue>
#include "Defs.h"
#include <glm/vec3.hpp>
#include <Thread>
#include <atomic>
#include <mutex>

class Chunk;

class AsyncChunkWorker
{
public:
	AsyncChunkWorker::~AsyncChunkWorker();

public:
	void RequestTask(SharedPtr<class Chunk> chunk);
	void Start();

private:
	void DoWork();

private:
	std::queue<SharedPtr<class Chunk>> requestedTasks;
	std::thread workThread;
	std::mutex requestingTask;
	std::atomic_bool shouldWork{ true };
};


class AsyncChunkManager
{
public:
	void RequestTask(SharedPtr<class Chunk> chunk);
	void Start(int numOfWorkers = 4);

private:
	int lastJobIndex = 0;
	std::vector<SharedPtr<AsyncChunkWorker>> workers;
};
