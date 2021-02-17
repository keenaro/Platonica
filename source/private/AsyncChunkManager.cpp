#include "AsyncChunkManager.h"
#include "World.h"
#include "ChunkRegion.h"

AsyncChunkManager::~AsyncChunkManager()
{
	shouldWork = false;
	workThread.join();
}

void AsyncChunkManager::Start()
{
	workThread = std::thread(&AsyncChunkManager::DoWork, this);
}

void AsyncChunkManager::RequestTask(SharedPtr<Chunk> chunk)
{
	while (!requestingTask.try_lock()) {}
	requestedTasks.push(chunk);
	requestingTask.unlock();
}

void AsyncChunkManager::DoWork()
{
	while (shouldWork)
	{
		if (!requestedTasks.empty())
		{
			while (!requestingTask.try_lock()) {}
			SharedPtr<Chunk> currentChunkTask = requestedTasks.front();
			requestedTasks.pop();
			requestingTask.unlock();
			currentChunkTask->GenerateChunkData();
		}
	}
}