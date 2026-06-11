// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/MyWorldSubsystem.h"

void UMyWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection) // 다른 서브시스템을 참조하기 위한 collection
{
	Super::Initialize(Collection);
	
	UE_LOG(LogTemp, Warning, TEXT("MyWorldSubsystem Initialize"));
}

void UMyWorldSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("MyWorldSubsystem Deinitialize"));
	
	Pools.Empty();
	ActiveActorToPoolMap.Empty();
	
	Super::Deinitialize();
}

void UMyWorldSubsystem::InitializePools(const TArray<FPoolSetupData>& PoolDatas)
{
	for (const FPoolSetupData& Data : PoolDatas)
	{
		// 클래스가 지정되지 않았으면 스킵
		if (!Data.ActorClass) continue;

		FPoolContainer NewContainer;
		NewContainer.SetupData = Data;

		// 설정된 초기 개수만큼 미리 생성하여 컨테이너에 넣음
		for (int32 i = 0; i < Data.InitialSize; ++i)
		{
			if (AActor* NewActor = CreateNewActorForPool(Data))
			{
				NewContainer.AvailableActors.Add(NewActor);
			}
		}

		// 맵에 풀 등록
		Pools.Add(Data.PoolName, NewContainer);
	}
}

AActor* UMyWorldSubsystem::SpawnFromPool(FName PoolName, FTransform SpawnTransform)
{
	// 1. 해당 이름의 풀이 존재하는지 확인
	if (!Pools.Contains(PoolName))
	{
		UE_LOG(LogTemp, Warning, TEXT("PoolName [%s] does not exist!"), *PoolName.ToString());
		return nullptr;
	}

	FPoolContainer& Container = Pools[PoolName];
	AActor* ActorToSpawn = nullptr;

	// 2. 사용 가능한 액터가 있는지 확인 (배열의 마지막 요소를 꺼내는 것이 성능에 좋음)
	while (Container.AvailableActors.Num() > 0 && ActorToSpawn == nullptr)
	{
		ActorToSpawn = Container.AvailableActors.Pop();
	}

	// 3. 사용 가능한 액터가 없다면 새로 생성 (bCanGrow가 참일 때만)
	if (ActorToSpawn == nullptr)
	{
		if (Container.SetupData.bCanGrow)
		{
			ActorToSpawn = CreateNewActorForPool(Container.SetupData);
		}
		else
		{
			return nullptr; // 성장 불가 & 풀 꽉 참
		}
	}

	// 4. 액터가 준비되었다면 활성화 및 맵 등록
	if (ActorToSpawn)
	{
		ActiveActorToPoolMap.Add(ActorToSpawn, PoolName);

		ActorToSpawn->SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);
		ActorToSpawn->SetActorHiddenInGame(false);
		ActorToSpawn->SetActorEnableCollision(true);
		ActorToSpawn->SetActorTickEnabled(true);
	}

	return ActorToSpawn;
}

void UMyWorldSubsystem::ReturnToPool(AActor* ActorToReturn)
{
	if (!ActorToReturn) return;

	// 1. 이 액터가 우리 풀 시스템에서 관리되는 액터인지 확인
	FName* FoundPoolName = ActiveActorToPoolMap.Find(ActorToReturn);
	if (!FoundPoolName || !Pools.Contains(*FoundPoolName))
	{
		// 풀에서 관리하는 액터가 아니면 그냥 파괴해버림
		ActorToReturn->Destroy();
		return;
	}

	// 2. 액터 비활성화
	ActorToReturn->SetActorHiddenInGame(true);
	ActorToReturn->SetActorEnableCollision(false);
	ActorToReturn->SetActorTickEnabled(false);

	// 3. 관리 맵에서 제거하고 사용 가능(Available) 목록으로 돌려보냄
	Pools[*FoundPoolName].AvailableActors.Add(ActorToReturn);
	ActiveActorToPoolMap.Remove(ActorToReturn);
}

AActor* UMyWorldSubsystem::CreateNewActorForPool(const FPoolSetupData& SetupData)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedActor = World->SpawnActor<AActor>(SetupData.ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	
	if (SpawnedActor)
	{
		SpawnedActor->SetActorHiddenInGame(true);
		SpawnedActor->SetActorEnableCollision(false);
		SpawnedActor->SetActorTickEnabled(false);
	}

	return SpawnedActor;
}


