// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Engine.h"
#include <vector>
#include "Math/Vector.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "Math/UnrealMathUtility.h"
#include <chrono>
#include "DrawDebugHelpers.h"
#include <thread>

#include "DestructibleManager.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DESTRUCTIONTEST_API UDestructibleManager : public USceneComponent
{
	GENERATED_BODY()


public:
	struct instanceRef
	{
		int instanceIndex;
		int index;
	};


	struct Chunk
	{
		int index;
		FVector position;
		std::vector<instanceRef> instanceReferences;
		float health;
		bool empty;
		std::vector<Chunk*> connectedChunks;
	};

	std::vector<Chunk*> allChunks;
	TSet<UStaticMeshComponent*> currentChunksSet;
	TSet<UStaticMeshComponent*> allChunksSet;

	int numRemoved0 = 0;
	int numRemoved1 = 0;
	int numRemoved2 = 0;
	int numRemoved3 = 0;

	FVector buildingSize;


public:


	void AddChunk(FVector position, std::vector<instanceRef> chunkReferences, float health, bool empty);

	UFUNCTION(BlueprintCallable, Category = "DestructibleManager")
		void GenerateLinks(float maxDistance, bool overwrite, FString linkFileName);

	UFUNCTION(BlueprintCallable, Category = "DestructibleManager")
		void FindAdjacentChunks(FVector hitLocation, int damageRadius, TArray<UInstancedStaticMeshComponent*> components);

	void CheckLinks(std::vector<UDestructibleManager::Chunk*>* nextLayerChunks, TArray<UInstancedStaticMeshComponent*> components);

	void ExtraRemoveLinks(UDestructibleManager::Chunk* chunk, TArray<UInstancedStaticMeshComponent*> components);


	//void CheckLinks()

public:
	// Sets default values for this component's properties
	UDestructibleManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
