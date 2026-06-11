// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/TimeWorldSubsystem.h"

int32 UTimeWorldSubsystem::GetCurrentHour()
{
	UWorld* World = GetWorld();
	if (World)
	{
		float TotalSeconds = World->GetTimeSeconds();
		
		int32 Hours = FMath::FloorToInt(TotalSeconds / 60);
		
		return Hours%24;
	}
	
	return 0;
}
