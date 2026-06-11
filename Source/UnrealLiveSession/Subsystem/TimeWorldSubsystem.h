// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TimeWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UNREALLIVESESSION_API UTimeWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION()
	int32 GetCurrentHour();
};
