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

void AsyncChunkManager::MergeCompletedTasks(World& world, bool mergeAll)
{
	if (completingTask.try_lock())
	{
		while(!completedTasks.empty())
		{
			ChunkTask task = completedTasks.front();
			world.GetOrCreateRegion(task.regionPosition)->InsertChunk(task.chunk);
			completedTasks.pop();

			if (!mergeAll) break;
		}

		completingTask.unlock();
	}
}

void AsyncChunkManager::RequestTask(glm::ivec3& regionPosition, glm::ivec3& chunkPosition)
{
	requestedTasks.push_back(ChunkTask(regionPosition, chunkPosition));
}

void AsyncChunkManager::DoWork()
{
	while (shouldWork)
	{
		if (!requestedTasks.empty())
		{
			while (!completingTask.try_lock()) {}
			ChunkTask currentTask = requestedTasks.front();
			currentTask.chunk = MakeShared<Chunk>(currentTask.chunkPosition);
			currentTask.chunk->GenerateChunkData();
			completedTasks.push(currentTask);
			completingTask.unlock();

			requestedTasks.pop_front();
		}
	}
}