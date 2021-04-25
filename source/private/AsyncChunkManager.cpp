#include "World.h"
#include "AsyncChunkManager.h"
#include "ChunkRegion.h"
#include "GameManager.h"

void GenerateJob::StartJob()
{
	if (chunk.use_count() > 1)
	{
		chunk->GenerateChunkData();
	}
}

void SaveJob::StartJob()
{
	chunk->SaveChunkData();
}

bool SaveJob::IsJobValid() const
{
	return chunk->IsLoaded() && GameManager::Instance().GetWorld()->IsHostWorld();
}

void ClientGenerateJob::StartJob()
{
	if (chunk.use_count() > 1)
	{
		const int dataSize = chunkData.size();
		chunk->GenerateChunkData(dataSize ? &chunkData.front() : nullptr, dataSize);
	}
}

AsyncChunkManager::AsyncChunkManager(int numOfWorkers)
{
	for (int i = 0; i < numOfWorkers; i++)
	{
		SharedPtr<AsyncChunkWorker> newWorker = MakeShared<AsyncChunkWorker>();
		newWorker->Start();
		workers.push_back(newWorker);
	}
}

void AsyncChunkManager::Exit()
{
	bool canExit = false;
	while (!canExit)
	{
		canExit = true;
		for (int i = 0; i < workers.size(); i++)
		{
			if(!workers[i]->TryExit())
			{
				canExit = false;
			}
		}
	}
}

bool AsyncChunkWorker::TryExit()
{	
	if(!requestedTasks.size())
	{
		shouldWork = false;
		if (workThread.joinable()) workThread.join();

		return true;
	}

	return false;
}


void AsyncChunkManager::RequestTask(SharedPtr<ChunkJob> chunkJob)
{
	if (workers.size() > 0)
	{
		if (chunkJob->IsJobValid())
		{
			workers[lastJobIndex]->RequestTask(chunkJob);
			if (++lastJobIndex > workers.size() - 1) lastJobIndex = 0;
		}
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
			SharedPtr<ChunkJob> currentChunkJob = requestedTasks.front();
			requestedTasks.pop();
			requestingTask.unlock();

			currentChunkJob->StartJob();
		}
		else
		{
			Sleep(10);
		}
	}
}
