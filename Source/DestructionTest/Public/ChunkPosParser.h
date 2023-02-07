// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DestructibleManager.h"

#include "ChunkPosParser.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DESTRUCTIONTEST_API UChunkPosParser : public USceneComponent
{
	GENERATED_BODY()

public:

	UDestructibleManager* manager;

	UFUNCTION(BlueprintCallable, Category = "ChunkParser")
		void Init(FString path, TArray<UInstancedStaticMeshComponent*> components, UDestructibleManager* destructibleManager);

	void AddDestructibleChunks(bool empty, bool window, FVector initPosition, int componentIndex, int length);





	UChunkPosParser();

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


		
};
