// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MyWorldSubsystem.generated.h"

/**
 * 
 */

// 풀 초기화를 위한 설정 데이터 구조체
USTRUCT(BlueprintType)
struct FPoolSetupData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Settings")
	FName PoolName; // 풀 이름 (예: "Bullet", "Effect")

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Settings")
	TSubclassOf<AActor> ActorClass; // 생성할 액터 클래스

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Settings")
	int32 InitialSize = 10; // 초기 생성 개수

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Settings")
	bool bCanGrow = true; // 풀이 꽉 찼을 때 추가 생성 허용 여부
};

// 내부적으로 액터들을 담아둘 컨테이너
USTRUCT()
struct FPoolContainer
{
	GENERATED_BODY()

	// 현재 사용 가능한 액터들
	UPROPERTY()
	TArray<AActor*> AvailableActors;

	// 이 풀의 설정값 (클래스, 확장 가능 여부 등)
	FPoolSetupData SetupData;
};

UCLASS()
class UNREALLIVESESSION_API UMyWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// 서브시스템의 시작 과 끝
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	// 1. 게임 시작 시 풀들을 세팅합니다.
	UFUNCTION(BlueprintCallable, Category = "ObjectPool")
	void InitializePools(const TArray<FPoolSetupData>& PoolDatas);

	// 2. 풀에서 액터를 꺼냅니다. (원하는 위치/회전값 적용)
	UFUNCTION(BlueprintCallable, Category = "ObjectPool")
	AActor* SpawnFromPool(FName PoolName, FTransform SpawnTransform);

	// 3. 다 쓴 액터를 풀로 반납합니다.
	UFUNCTION(BlueprintCallable, Category = "ObjectPool")
	void ReturnToPool(AActor* ActorToReturn);

private:
	// 내부적으로 새 액터를 생성하고 비활성화하는 헬퍼 함수
	AActor* CreateNewActorForPool(const FPoolSetupData& SetupData);

	// 이름(FName)으로 풀 컨테이너를 관리하는 맵
	UPROPERTY()
	TMap<FName, FPoolContainer> Pools;

	// 현재 활성화된 액터가 어느 풀 소속인지 역추적하기 위한 맵
	UPROPERTY()
	TMap<AActor*, FName> ActiveActorToPoolMap;
};