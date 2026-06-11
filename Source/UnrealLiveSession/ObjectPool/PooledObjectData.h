// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledObjectData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType) // 구조체로 제작한다
struct FPooledObjectData
{
	GENERATED_BODY()
	
	FPooledObjectData()
	{
		ActorTemplate = nullptr;
		PoolSize = 1;
		bCanGrow = false;
		ActorName = "Default";
	}
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorTemplate;
	
	UPROPERTY(EditAnywhere)
	int32 PoolSize;
	
	UPROPERTY(EditAnywhere)
	bool bCanGrow;
	
	UPROPERTY(EditAnywhere)
	FString ActorName;
};
