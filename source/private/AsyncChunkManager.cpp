#include "AsyncChunkManager.h"
#include "World.h"
#include "ChunkRegion.h"

AsyncChunkManager::AsyncChunkManager(int numOfWorkers)
{
	for (int i = 0; i < numOfWorkers; i++)
	{
		SharedPtr<AsyncChunkWorker> newWorker = MakeShared<AsyncChunkWorker>();
		newWorker->Start();
		workers.push_back(newWorker);
	}
}

AsyncChunkWorker::~AsyncChunkWorker()
{
	shouldWork = false;
	workThread.join();
}

void AsyncChunkManager::RequestTask(const SharedPtr<Chunk> chunk, const TaskType& taskType)
{
	if (workers.size() > 0)
	{
		if (taskType == TaskType::Save && !chunk->IsLoaded())
		{
			DPrint("Trying to save chunk that has not been loaded yet. Skipping.");
			return;
		}

		workers[lastJobIndex]->RequestTask(MakeShared<ChunkJob>(chunk, taskType));
		if (++lastJobIndex > workers.size() - 1) lastJobIndex = 0;
	}
	else
	{
		DPrint("Chunk Manager has already shut down.");
	}
}

void AsyncChunkWorker::RequestTask(const SharedPtr<ChunkJob> chunkJob)
{
	while (!requestingTask.try_lock()) {}
	requestedTasks.push(chunkJob);
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
			SharedPtr<ChunkJob> currentChunkTask = requestedTasks.front();
			requestedTasks.pop();
			requestingTask.unlock();

			switch(currentChunkTask->taskType)
			{
				case Generate: 
					if (currentChunkTask->chunk.use_count() > 1)
					{
						if (!currentChunkTask->chunk->TryLoadChunkData())
						{
							currentChunkTask->chunk->GenerateChunkData();
						}
					}
					break;
				case Save:
					currentChunkTask->chunk->SaveChunkData();
					break;
			}
		}
		else
		{
			Sleep(10);
		}
	}
}
