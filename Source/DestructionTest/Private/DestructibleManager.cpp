#include "DestructibleManager.h"


using namespace std;

void UDestructibleManager::AddChunk(FVector position, std::vector<instanceRef> chunkReferences, float health, bool empty)
{
	Chunk* tempChunk = new Chunk();
	tempChunk->position = position;
	tempChunk->instanceReferences = chunkReferences;
	tempChunk->health = health;
	tempChunk->empty = empty;
	tempChunk->index = allChunks.size();
	allChunks.push_back(tempChunk);
}


void UDestructibleManager::GenerateLinks(float maxDistance, bool overwrite, FString linkFileName)
{
	if (overwrite)
	{
		std::ofstream outfile;
		outfile.open(std::string(TCHAR_TO_UTF8(*linkFileName)) + ".lnks");

		for (int i = 0; i < allChunks.size(); i++)
		{
			for (int j = 0; j < allChunks.size(); j++)
			{
				if (i != j)
				{
					if (allChunks[i]->empty == false && allChunks[j]->empty == false)
					{
						float dist = (std::abs(allChunks[i]->position.X - allChunks[j]->position.X) + std::abs(allChunks[i]->position.Y - allChunks[j]->position.Y) + std::abs(allChunks[i]->position.Z - allChunks[j]->position.Z));
						if (dist <= maxDistance)
						{
							allChunks[i]->connectedChunks.push_back(allChunks[j]);
							outfile << i << " " << j << std::endl;
						}
					}
				}
			}
		}
		outfile.close();
	}
	else
	{
		std::ifstream infile(std::string(TCHAR_TO_UTF8(*linkFileName)) + ".lnks");
		std::string line;
		if (infile.is_open())
		{
			int g[2] = { 0,0 };
			while (std::getline(infile, line))
			{
				std::string substring;
				std::stringstream stream(line);
				for (int i = 0; i < 2; i++)
				{
					std::getline(stream, substring, ' ');
					g[i] = std::stoi(substring);
				}
				allChunks[g[0]]->connectedChunks.push_back(allChunks[g[1]]);
			}
			infile.close();
		}
	}
}

void UDestructibleManager::FindAdjacentChunks(FVector hitLocation, int damageRadius, TArray<UInstancedStaticMeshComponent*> components)
{
	auto start = chrono::steady_clock::now();
	FVector worldChunkPos = allChunks[0]->position + this->GetOwner()->GetActorLocation();
	FVector firstVector = (allChunks[0]->position / 100.0F);
	FVector lastVector = (allChunks[allChunks.size() - 1]->position / 100.0F);

	FVector localHit = ((hitLocation - worldChunkPos) / 100.0F) + firstVector;
	FVector localHitRound = FVector(std::roundf(localHit.X), std::roundf(localHit.Y), std::roundf(localHit.Z));
	localHitRound = FVector(std::fmaxf(std::fminf(localHitRound.X, lastVector.X), firstVector.X), std::fmaxf(std::fminf(localHitRound.Y, lastVector.Y), firstVector.Y), std::fmaxf(std::fminf(localHitRound.Z, lastVector.Z), firstVector.Z));

	int index = (((localHitRound.X + 1.0F) - firstVector.X) + (((localHitRound.Y + 0.0F) - firstVector.Y) * buildingSize.X) + (((localHitRound.Z + 0.0F) - firstVector.Z) * buildingSize.X * buildingSize.Y)) - 1; //-1 for NikitaMode(file starts from 1, array starts from 0)


	int size = (int)(buildingSize.X * buildingSize.Y * buildingSize.Z);
	if (allChunks[index]->empty == true)
	{
		int newIndex = 0;
		float minDistance = 100000;
		for (int i = 0; i < allChunks.size(); i++)
		{
			if (i != index && allChunks[i]->empty == false)
			{
				float tempDistance = FVector::Dist(allChunks[index]->position, allChunks[i]->position);
				if (tempDistance < minDistance)
				{
					minDistance = tempDistance;
					newIndex = i;
				}
			}
		}
		index = newIndex;
	}

	std::vector<Chunk*> NewChunks;
	std::vector<Chunk*> TempNewChunks;
	std::vector<Chunk*> PermanentChunks;
	std::vector<Chunk*> CheckLinksLayer;

	NewChunks.push_back(allChunks[index]);


	for (int d = 0; d < damageRadius + 1; d++)
	{
		TempNewChunks = NewChunks;
		NewChunks.clear();

		for (int i = 0; i < TempNewChunks.size(); i++)
		{
			Chunk* tempChunk = TempNewChunks[i];
			for (int j = 0; j < (*tempChunk).connectedChunks.size(); j++)
			{
				Chunk* tempFoundChunk = (*tempChunk).connectedChunks[j];
				if (std::find(NewChunks.begin(), NewChunks.end(), tempFoundChunk) == NewChunks.end()) //NewChunks DOES NOT contain tempFoundChunk
				{
					if (std::find(PermanentChunks.begin(), PermanentChunks.end(), tempFoundChunk) == PermanentChunks.end()) //PermanentChunks DOES NOT contain tempFoundChunk
					{
						NewChunks.push_back(tempFoundChunk);
					}
				}
			}
		}

		for (int i = 0; i < TempNewChunks.size(); i++)
		{
			if (std::find(PermanentChunks.begin(), PermanentChunks.end(), TempNewChunks[i]) == PermanentChunks.end()) //PermanentChunks DOES NOT contain TempNewChunks
			{
				PermanentChunks.push_back(TempNewChunks[i]);
				Chunk* tempChunk = TempNewChunks[i];
				if (d < damageRadius - 1)
				{
					for (int g = 0; g < (*tempChunk).instanceReferences.size(); g++)
					{
						FTransform* tempGetTransform = new FTransform();
						components[(*tempChunk).instanceReferences[g].instanceIndex]->GetInstanceTransform((*tempChunk).instanceReferences[g].index, (*tempGetTransform), false);
						FTransform tempNewTransform = FTransform((*tempGetTransform).Rotator(), (*tempGetTransform).GetTranslation(), FVector(0.0F, 0.0F, 0.0F));
						components[(*tempChunk).instanceReferences[g].instanceIndex]->UpdateInstanceTransform((*tempChunk).instanceReferences[g].index, tempNewTransform, false, true, true);
					}
					//remove links
					for (int j = 0; j < (*tempChunk).connectedChunks.size(); j++)
					{
						Chunk* otherChunk = (*tempChunk).connectedChunks[j];
						for (int k = 0; k < (*otherChunk).connectedChunks.size(); k++)
						{
							if ((*otherChunk).connectedChunks[k] == tempChunk)
							{
								(*otherChunk).connectedChunks.erase((*otherChunk).connectedChunks.begin() + k);
							}
						}
					}
				}
				else if (d < damageRadius)
				{
					int num = 0;
					if ((*tempChunk).instanceReferences.size() == 2) //if window
					{
						num = 1;
					}
					else if ((*tempChunk).instanceReferences.size() == 4) //if regular block
					{
						int rnd = FMath::RandRange(0, 100);
						num = 3;
						if (rnd > 75)
						{
							num = 2;
						}
						else if (rnd > 50)
						{
							num = 1;
						}
					}


					for (int g = 0; g < num; g++)
					{
						FTransform* tempGetTransform = new FTransform();
						components[(*tempChunk).instanceReferences[g].instanceIndex]->GetInstanceTransform((*tempChunk).instanceReferences[g].index, (*tempGetTransform), false);
						FTransform tempNewTransform = FTransform((*tempGetTransform).Rotator(), (*tempGetTransform).GetTranslation(), FVector(0.0F, 0.0F, 0.0F));
						components[(*tempChunk).instanceReferences[g].instanceIndex]->UpdateInstanceTransform((*tempChunk).instanceReferences[g].index, tempNewTransform, false, true, true);
					}
				}
				else //next layer for CheckLinks
				{
					CheckLinksLayer.push_back(tempChunk);
				}
			}
		}
		TempNewChunks.clear();
	}
	auto end = chrono::steady_clock::now();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("FindAdjacent: %d"), std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()));

	CheckLinks(&CheckLinksLayer, components);
}



void UDestructibleManager::CheckLinks(std::vector<UDestructibleManager::Chunk*>* nextLayerChunks, TArray<UInstancedStaticMeshComponent*> components)
{
	std::vector<Chunk*> NewChunks;
	std::vector<Chunk*> TempNewChunks;
	std::vector<Chunk*> PermanentChunks;

	auto startRoot = chrono::steady_clock::now();
	//add all chunks from the first layer to NewChunks(root chunks)
	for (int i = 1; i <= buildingSize.X; i++)
	{
		for (int j = 1; j <= buildingSize.Y; j++)
		{
			NewChunks.push_back((allChunks[(i * j) - 1]));
		}
	}
	auto endRoot = std::chrono::steady_clock::now();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CheckLinks FindRoot: %d"), std::chrono::duration_cast<std::chrono::milliseconds>(endRoot - startRoot).count()));



	//create and reset bigArray
	int size = (int)(buildingSize.X * buildingSize.Y * buildingSize.Z);
	bool* bigArray = (bool*)malloc(size);

	for (int i = 0; i < size; i++)
	{
		bigArray[i] = false;
	}


	auto startLinked = chrono::steady_clock::now();
	//find all linked chunks
	while (NewChunks.size() > 0)
	{
		TempNewChunks = NewChunks;
		NewChunks.clear();

		for (int i = 0; i < TempNewChunks.size(); i++)
		{
			Chunk* tempChunk = TempNewChunks[i];
			bigArray[TempNewChunks[i]->index] = true;
			for (int j = 0; j < (*tempChunk).connectedChunks.size(); j++)
			{
				Chunk* tempFoundChunk = (*tempChunk).connectedChunks[j];
				if (bigArray[tempFoundChunk->index] == false) //NewChunks DOES NOT contain tempFoundChunk
				{
					NewChunks.push_back(tempFoundChunk);
					bigArray[tempFoundChunk->index] = true;
				}
			}
		}
	}
	for (int i = 0; i < size; i++)
	{
		if (bigArray[i] == false)
		{
			//remove links
			Chunk* tempChunk = allChunks[i];
			for (int j = 0; j < (*tempChunk).connectedChunks.size(); j++)
			{
				Chunk* otherChunk = (*tempChunk).connectedChunks[j];
				for (int k = 0; k < (*otherChunk).connectedChunks.size(); k++)
				{
					if ((*otherChunk).connectedChunks[k] == tempChunk)
					{
						(*otherChunk).connectedChunks.erase((*otherChunk).connectedChunks.begin() + k);
					}
				}
			}

			for (int g = 0; g < (*tempChunk).instanceReferences.size(); g++)
			{
				FTransform* tempGetTransform = new FTransform();
				components[(*tempChunk).instanceReferences[g].instanceIndex]->GetInstanceTransform((*tempChunk).instanceReferences[g].index, (*tempGetTransform), false);
				FTransform tempNewTransform = FTransform((*tempGetTransform).Rotator(), (*tempGetTransform).GetTranslation(), FVector(0.0F, 0.0F, 0.0F));
				components[(*tempChunk).instanceReferences[g].instanceIndex]->UpdateInstanceTransform((*tempChunk).instanceReferences[g].index, tempNewTransform, false, true, true);
			}
		}
	}

	free(bigArray);

	auto endLinked = std::chrono::steady_clock::now();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CheckLinks FindLinked: %d"), std::chrono::duration_cast<std::chrono::milliseconds>(endLinked - startLinked).count()));



	auto start1 = std::chrono::steady_clock::now();

	auto end1 = std::chrono::steady_clock::now();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CheckLinks FindNextLayer: %d"), std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count()));
}

void UDestructibleManager::ExtraRemoveLinks(UDestructibleManager::Chunk* chunk, TArray<UInstancedStaticMeshComponent*> components)
{
	for (int i = 0; i < (*chunk).connectedChunks.size(); i++)
	{
		Chunk* otherChunk = (*chunk).connectedChunks[i];
		for (int j = 0; j < (*otherChunk).connectedChunks.size(); j++)
		{
			if ((*otherChunk).connectedChunks[j] == chunk)
			{
				(*otherChunk).connectedChunks.erase((*otherChunk).connectedChunks.begin() + j);
			}
		}
	}
	for (int g = 0; g < (*chunk).instanceReferences.size(); g++)
	{
		FTransform* tempGetTransform = new FTransform();
		components[(*chunk).instanceReferences[g].instanceIndex]->GetInstanceTransform((*chunk).instanceReferences[g].index, (*tempGetTransform), false);
		FTransform tempNewTransform = FTransform((*tempGetTransform).Rotator(), (*tempGetTransform).GetTranslation(), FVector(0.0F, 0.0F, 0.0F));
		components[(*chunk).instanceReferences[g].instanceIndex]->UpdateInstanceTransform((*chunk).instanceReferences[g].index, tempNewTransform, false, true, true);
	}
}

// Sets default values for this component's properties
UDestructibleManager::UDestructibleManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UDestructibleManager::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UDestructibleManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
