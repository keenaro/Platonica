#include "AsyncChunkManager.h"
#include "World.h"
#include "ChunkRegion.h"

void AsyncChunkManager::Start(int numOfWorkers)
{
	for (int i = 0; i < numOfWorkers; i++)
	{
		SharedPtr<AsyncChunkWorker> newWorker = MakeShared<AsyncChunkWorker>();
		newWorker->Start();
		workers.push_back(newWorker);
	}
}

void AsyncChunkManager::RequestTask(SharedPtr<Chunk> chunk)
{
	workers[lastJobIndex]->RequestTask(chunk);
	if (++lastJobIndex > workers.size() - 1) lastJobIndex = 0;
}

void AsyncChunkWorker::RequestTask(SharedPtr<Chunk> chunk)
{
	while (!requestingTask.try_lock()) {}
	requestedTasks.push(chunk);
	requestingTask.unlock();
}

void AsyncChunkWorker::Start()
{
	workThread = std::thread(&AsyncChunkWorker::DoWork, this);
}

void AsyncChunkWorker::DoWork()
{
	while (shouldWork)
	{
		if (!requestedTasks.empty())
		{
			while (!requestingTask.try_lock()) {}
			SharedPtr<Chunk> currentChunkTask = requestedTasks.front();
			requestedTasks.pop();
			requestingTask.unlock();
			if (currentChunkTask.use_count() > 1)
			{
				currentChunkTask->GenerateChunkData();
			}
		}
		else
		{
			Sleep(10);
		}
	}
}

AsyncChunkWorker::~AsyncChunkWorker()
{
	shouldWork = false;
	workThread.join();
}