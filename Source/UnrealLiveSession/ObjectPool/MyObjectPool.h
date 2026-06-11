// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectPool/PooledObjectData.h"

#include "MyObjectPool.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPoolerCleanupSignature); //델리게이트

USTRUCT(BlueprintType)
struct FSingleObjectPool
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<TObjectPtr<class UPooledObject>> PooledObjects;
};


UCLASS()
class UNREALLIVESESSION_API AMyObjectPool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyObjectPool();
	
	UPROPERTY()
	FPoolerCleanupSignature OnPoolerCleanup;
	
	// 소환한 액터들 이름과 매칭해서 가져가기
	UFUNCTION(BlueprintCallable)
	AActor* GetPooledActor(FString Name);
	
	//액터를 넘겨주면 회수할 수 있도록 만들어주기
	UFUNCTION(BlueprintCallable)
	void RecycleActor(AActor* PooledActor);
	
	// 최종 회수
	UFUNCTION(BlueprintCallable)
	void RecyclePooledObject(class UPooledObject* PoolCompRef);
	
	// 델리게이트 호출
	UFUNCTION(BlueprintCallable)
	void Broadcast_PooledCleanup();
	
	//밖에서 생성할 때 지정해주는 구조체
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPooledObjectData> PooledObjectData;
	
protected:
	// 액터에 붙어있는 컴포넌트를 모아두는 장소
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FSingleObjectPool> Pools;
	
private:
	
	// 즉시 만들기
	void Regenitem(int32 PoolIndex, int32 PositionIndex);
	

public:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
