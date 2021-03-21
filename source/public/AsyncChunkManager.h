#pragma once
#include <deque>
#include <queue>
#include "Defs.h"
#include <glm/vec3.hpp>
#include <Thread>
#include <atomic>
#include <mutex>

class Chunk;

enum TaskType 
{
	Generate,
	Save
};

struct ChunkJob
{
public:
	ChunkJob(SharedPtr<Chunk> newChunk, TaskType newTaskType) : chunk(newChunk), taskType(newTaskType) {};
	SharedPtr<Chunk> chunk;
	TaskType taskType;
};


class AsyncChunkWorker
{
public:
	AsyncChunkWorker::~AsyncChunkWorker();

public:
	void RequestTask(const SharedPtr<ChunkJob> chunkJob);
	void Start();

private:
	void DoWork();

private:
	std::queue<SharedPtr<ChunkJob>> requestedTasks;
	std::thread workThread;
	std::mutex requestingTask;
	std::atomic_bool shouldWork{ true };
};


class AsyncChunkManager
{
public:
	void RequestTask(const SharedPtr<Chunk> chunk, const TaskType& taskType);
	AsyncChunkManager(int numOfWorkers = 4);

private:
	int lastJobIndex = 0;
	std::vector<SharedPtr<AsyncChunkWorker>> workers;
};
