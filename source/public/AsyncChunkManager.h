#pragma once
#include <deque>
#include <queue>
#include <glm/vec3.hpp>
#include <Thread>
#include <atomic>
#include <mutex>
#include "Defs.h"

class Chunk;

enum TaskType 
{
	Generate,
	Save
};

class ChunkJob
{
public:
	ChunkJob(SharedPtr<Chunk> newChunk) : chunk(newChunk) {};
	virtual void StartJob() = 0;
	virtual bool IsJobValid() const { return true; };

protected:
	SharedPtr<Chunk> chunk;

};

class GenerateJob : public ChunkJob
{
public:
	GenerateJob(SharedPtr<Chunk> newChunk) : ChunkJob(newChunk) {};
	void StartJob() override;
};

class SaveJob : public ChunkJob
{
public:
	SaveJob(SharedPtr<Chunk> newChunk) : ChunkJob(newChunk) {};
	void StartJob() override;
	bool IsJobValid() const override;
};

class ClientGenerateJob : public ChunkJob
{
public:
	ClientGenerateJob(SharedPtr<Chunk> newChunk, const std::vector<ChunkBlockData>& receivedData) : ChunkJob(newChunk), chunkData(receivedData) {};

	void StartJob() override;

protected:
	std::vector<ChunkBlockData> chunkData;
};


class AsyncChunkWorker
{
public:
	void RequestTask(const SharedPtr<ChunkJob> chunkJob);
	void Start();
	bool TryExit();
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
	void RequestTask(SharedPtr<ChunkJob> chunkJob);
	AsyncChunkManager(int numOfWorkers = 4);
	void Exit();

private:
	int lastJobIndex = 0;
	std::vector<SharedPtr<AsyncChunkWorker>> workers;
};
