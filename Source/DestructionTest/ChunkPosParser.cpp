// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkPosParser.h"

#define COLOR1 170
#define COLOR2 85
#define COLOR3 34
#define EMPTY 100
#define FRONT 180
#define RIGHT 200
#define BACK 255
#define LEFT 0



void UChunkPosParser::Init(FString path, TArray<UInstancedStaticMeshComponent*> components, UDestructibleManager* destructibleManager)
{
	manager = destructibleManager;

	std::string stringPath = std::string(TCHAR_TO_UTF8(*path));
	srand((unsigned)time(0));
	std::string line;
	std::ifstream file(stringPath);
	if (file.is_open())
	{
		//parse building size
		std::stringstream sizeStream(stringPath);
		std::string sizeString;
		std::getline(sizeStream, sizeString, '(');
		std::getline(sizeStream, sizeString, '(');//repeat to get part2

		std::string sizeFString;
		std::stringstream sizeFStream(sizeString);
		std::getline(sizeFStream, sizeFString, ')');

		std::stringstream sizeCStream(sizeFString);
		std::string xS;
		std::string yS;
		std::string zS;
		std::getline(sizeCStream, xS, ',');
		std::getline(sizeCStream, yS, ',');
		std::getline(sizeCStream, zS, ',');
		FVector finalSize = FVector(std::stof(xS), std::stof(yS), std::stof(zS));
		destructibleManager->buildingSize = finalSize;




		while (std::getline(file, line))
		{
			std::stringstream stream(line);

			FVector newPos;
			std::string substring;
			for (int i = 0; i < 4; i++)
			{
				std::getline(stream, substring, ' ');

				switch (i)
				{
				case 0:
					newPos.X = 100.0F * std::stof(substring);
					break;
				case 1:
					newPos.Y = 100.0F * std::stof(substring);
					break;
				case 2:
					newPos.Z = 100.0F * std::stof(substring);
					break;
				case 3:
					float randX = ((rand() % 1) + 1) * 90.0F;
					float randY = (rand() % 3) * 90.0F;
					float randZ = (rand() % 3) * 90.0F;

					FRotator newRot1 = FRotator(90.0F + randX, 90.0F + randY, 0.0F + randZ);
					FRotator newRot2 = FRotator(90.0F + randX, 90.0F + randY, 180.0F + randZ);
					FRotator newRot3 = FRotator(270.0F + randX, 90.0F + randY, 0.0F + randZ);
					FRotator newRot4 = FRotator(90.0F + randX, 270.0F + randY, 0.0F + randZ);


					FTransform* newTransform1 = new FTransform(newRot1, newPos, FVector(8.467F, 8.467F, 8.467F));
					FTransform* newTransform2 = new FTransform(newRot2, newPos, FVector(8.467F, 8.467F, 8.467F));
					FTransform* newTransform3 = new FTransform(newRot3, newPos, FVector(8.467F, 8.467F, 8.467F));
					FTransform* newTransform4 = new FTransform(newRot4, newPos, FVector(8.467F, 8.467F, 8.467F));



					int randomInt = 0;
					//Parse Color
					switch (std::stoi(substring))
					{
					case EMPTY:
						AddDestructibleChunks(true, false, newPos, 0, 0);
						break;
					case COLOR1:
						randomInt = (rand() % 6) + 1;
						components[0]->AddInstance(*newTransform1);
						components[0]->AddInstance(*newTransform2);
						components[0]->AddInstance(*newTransform3);
						components[0]->AddInstance(*newTransform4);
						AddDestructibleChunks(false, false, newPos, 0, components[0]->GetInstanceCount());
						break;
					case COLOR2:
						randomInt = (rand() % 6) + 1;
						components[1]->AddInstance(*newTransform1);
						components[1]->AddInstance(*newTransform2);
						components[1]->AddInstance(*newTransform3);
						components[1]->AddInstance(*newTransform4);
						AddDestructibleChunks(false, false, newPos, 1, components[1]->GetInstanceCount());
						break;
					case COLOR3:
						randomInt = (rand() % 6) + 1;
						components[2]->AddInstance(*newTransform1);
						components[2]->AddInstance(*newTransform2);
						components[2]->AddInstance(*newTransform3);
						components[2]->AddInstance(*newTransform4);
						AddDestructibleChunks(false, false, newPos, 2, components[2]->GetInstanceCount());
						break;
					case FRONT:
						components[3]->AddInstance(FTransform(FRotator(0.0F, 90.0F, 0.0F), newPos, FVector(8.467F, 8.467F, 8.467F)));
						components[3]->AddInstance(FTransform(FRotator(0.0F, 90.0F, 180.0F), newPos, FVector(8.467F, 8.467F, 8.467F)));
						AddDestructibleChunks(false, true, newPos, 3, components[3]->GetInstanceCount());
						break;
					case RIGHT:
						components[3]->AddInstance(FTransform(FRotator(0.0F, 180.0F, 0.0F), newPos, FVector(8.467F, 8.467F, 8.467F)));
						components[3]->AddInstance(FTransform(FRotator(0.0F, 180.0F, 180.0F), newPos, FVector(8.467F, 8.467F, 8.467F)));
						AddDestructibleChunks(false, true, newPos, 3, components[3]->GetInstanceCount());
						break;
					case BACK:
						components[3]->AddInstance(FTransform(FRotator(0.0F, 270.0F, 0.0F), newPos, FVector(8.467F, 8.467F, 8.467F)));
						components[3]->AddInstance(FTransform(FRotator(0.0F, 270.0F, 180.0F), newPos, FVector(8.467F, 8.467F, 8.467F)));
						AddDestructibleChunks(false, true, newPos, 3, components[3]->GetInstanceCount());
						break;
					case LEFT:
						components[3]->AddInstance(FTransform(FRotator(0.0F, 0.0F, 0.0F), newPos, FVector(8.467F, 8.467F, 8.467F)));
						components[3]->AddInstance(FTransform(FRotator(0.0F, 0.0F, 180.0F), newPos, FVector(8.467F, 8.467F, 8.467F)));
						AddDestructibleChunks(false, true, newPos, 3, components[3]->GetInstanceCount());
						break;
					}
					break;
				}
			}
		}
		file.close();
	}
}



void UChunkPosParser::AddDestructibleChunks(bool empty, bool window, FVector initPosition, int componentIndex, int length)
{
	std::vector<UDestructibleManager::instanceRef> tempInsatanceReferences;
	if (empty)
	{
		manager->AddChunk(initPosition, tempInsatanceReferences, 0.0F, true);
	}
	else
	{
		int fuckRandom[2] = { -1,1 };
		float randX = fuckRandom[(rand() % 1)];
		float randY = fuckRandom[(rand() % 1)];
		float randZ = fuckRandom[(rand() % 1)];
		float offset = 50.0F;
		if (!window)
		{
			for (int i = 4; i > 0; i--)
			{
				UDestructibleManager::instanceRef tempRef;
				tempRef.instanceIndex = componentIndex;
				tempRef.index = length - i;
				tempInsatanceReferences.push_back(tempRef);
			}
			manager->AddChunk(initPosition, tempInsatanceReferences, 100.0F, false);
		}
		else
		{
			for (int i = 2; i > 0; i--)
			{
				UDestructibleManager::instanceRef tempRef;
				tempRef.instanceIndex = componentIndex;
				tempRef.index = length - i;
				tempInsatanceReferences.push_back(tempRef);
			}
			manager->AddChunk(initPosition, tempInsatanceReferences, 100.0F, false);
		}
	}
}




// Sets default values for this component's properties
UChunkPosParser::UChunkPosParser()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UChunkPosParser::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UChunkPosParser::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

