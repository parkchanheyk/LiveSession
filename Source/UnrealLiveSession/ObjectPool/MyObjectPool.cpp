// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/MyObjectPool.h"

#include "GeometryCollection/GeometryCollectionParticlesData.h"
#include "ObjectPool/PooledObject.h"

// Sets default values
AMyObjectPool::AMyObjectPool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyObjectPool::BeginPlay()
{
	Super::BeginPlay();
	
	FActorSpawnParameters SpawnParams;
	
	for (int32 PoolIndex = 0 ; PoolIndex < PooledObjectData.Num(); ++PoolIndex)
	{
		FSingleObjectPool CurrentpoolIndex;
		
		SpawnParams.Name = FName(FString::Printf(TEXT("%s"), *PooledObjectData[PoolIndex].ActorName));
		
		// 내가 생성한 이름을 사용해달라
		SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
		
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		for (int32 ObjectIndex = 0; ObjectIndex < PooledObjectData[PoolIndex].PoolSize; ++ObjectIndex)
		{
			// 스폰
			AActor* SpawnedActor = GetWorld()->SpawnActor(PooledObjectData[PoolIndex].ActorTemplate, &FVector::ZeroVector, &FRotator::ZeroRotator, SpawnParams);
			// 외부 이름 재설정
			SpawnedActor->SetActorLabel(SpawnedActor->GetName());
			//컴포넌트 생성
			UPooledObject* PoolComp = NewObject<UPooledObject>(SpawnedActor);
			//컴포넌트를 언리얼에 등록
			PoolComp->RegisterComponent(); 
			// 특정 액터의 소유
			SpawnedActor->AddInstanceComponent(PoolComp);
			//컴포넌트 초기화
			PoolComp->Init(this);
			
			
			CurrentpoolIndex.PooledObjects.Add(PoolComp);
			
			SpawnedActor->SetActorHiddenInGame(true);
			SpawnedActor->SetActorEnableCollision(false);
			SpawnedActor->SetActorTickEnabled(false);
			SpawnedActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
			
		}
		// 컴포넌트 뭉치 저장
		Pools.Add(CurrentpoolIndex);
	}
}

// Called every frame
void AMyObjectPool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AActor* AMyObjectPool::GetPooledActor(FString Name)
{
	int32 PoolCount = Pools.Num();
	
	int32 CurrentPoolIndex = -1;
	for (int32 i = 0; i < PoolCount; ++i)
	{
		if (PooledObjectData[i].ActorName == Name)
		{
			CurrentPoolIndex = i;
			break;
		}
	}
	
	if (CurrentPoolIndex == -1) return nullptr;
	
	int32 PooledObjectCount = Pools[CurrentPoolIndex].PooledObjects.Num();
	
	int32 FirstAvailable = -1;

	for (int32 i = 0; i < PooledObjectCount; ++i)
	{
		if (Pools[CurrentPoolIndex].PooledObjects[i] != nullptr)
		{
			//컴포넌트가 액티브 상태가 아니라면?
			if (Pools[CurrentPoolIndex].PooledObjects[i]->bIsPoolActive == false)
			{
				FirstAvailable = i;
				break;
			}
		}
		else
		{
			Regenitem(CurrentPoolIndex, i);
			FirstAvailable = i;
			break;
		}
	}
	
	if (FirstAvailable >= 0)
	{
		UPooledObject* ToReturn = Pools[CurrentPoolIndex].PooledObjects[FirstAvailable];
		ToReturn->bIsPoolActive = true;
		
		OnPoolerCleanup.AddUniqueDynamic(ToReturn, &UPooledObject::RecycleSelf);
		
		AActor* ActorToReturn = ToReturn->GetOwner();
		
		ActorToReturn->SetActorHiddenInGame(false);
		ActorToReturn->SetActorEnableCollision(true);
		ActorToReturn->SetActorTickEnabled(true);
		
		ActorToReturn->AttachToActor(nullptr, FAttachmentTransformRules::SnapToTargetIncludingScale);
		return ActorToReturn;
	}
	
	// bCanGrow가 false이면
	if (!PooledObjectData[CurrentPoolIndex].bCanGrow) return nullptr;
	
	// bCanGrow가 true이면
	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = FName(FString::Printf(TEXT("%s"), *PooledObjectData[CurrentPoolIndex].ActorName));
	SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	AActor* SpawnedActor = GetWorld()->SpawnActor(PooledObjectData[CurrentPoolIndex].ActorTemplate, &FVector::ZeroVector, &FRotator::ZeroRotator, SpawnParams);
	
	SpawnedActor->SetActorLabel(SpawnedActor->GetName());
	
	UPooledObject* PoolComp = NewObject<UPooledObject>(SpawnedActor);
	PoolComp->RegisterComponent();
	SpawnedActor->AddInstanceComponent(PoolComp);
	PoolComp->Init(this);
	
	Pools[CurrentPoolIndex].PooledObjects.Add(PoolComp);
	
	PoolComp->bIsPoolActive = true;
	OnPoolerCleanup.AddUniqueDynamic(PoolComp, &UPooledObject::RecycleSelf);
	return SpawnedActor;
}

void AMyObjectPool::RecycleActor(AActor* PooledActor)
{
	if (UPooledObject* PoolCompRef = Cast<UPooledObject>(PooledActor->GetComponentByClass(UPooledObject::StaticClass())))
	{
		RecyclePooledObject(PoolCompRef);
	}
}

void AMyObjectPool::RecyclePooledObject(class UPooledObject* PoolCompRef)
{
	OnPoolerCleanup.RemoveDynamic(PoolCompRef, &UPooledObject::RecycleSelf);

	PoolCompRef->bIsPoolActive = false;
	AActor* ReturningActor = PoolCompRef->GetOwner();
	ReturningActor->SetActorHiddenInGame(true);
	ReturningActor->SetActorEnableCollision(false);
	ReturningActor->SetActorTickEnabled(false);

	ReturningActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void AMyObjectPool::Broadcast_PooledCleanup()
{
	OnPoolerCleanup.Broadcast();
}

void AMyObjectPool::Regenitem(int32 PoolIndex, int32 PositionIndex)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = FName(FString::Printf(TEXT("%s"), *PooledObjectData[PoolIndex].ActorName));
	SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


	AActor* SpawnedActor = GetWorld()->SpawnActor(PooledObjectData[PoolIndex].ActorTemplate,&FVector::ZeroVector, &FRotator::ZeroRotator, SpawnParams);
	
	//외부 아웃라이너 이름 바꾸기
	SpawnedActor->SetActorLabel(SpawnedActor->GetName());

	UPooledObject* PoolComp = NewObject<UPooledObject>(SpawnedActor);
	PoolComp->RegisterComponent();
	SpawnedActor->AddInstanceComponent(PoolComp);
	PoolComp->Init(this);

	Pools[PoolIndex].PooledObjects.Insert(PoolComp, PositionIndex);
	SpawnedActor->SetActorHiddenInGame(true);
	SpawnedActor->SetActorEnableCollision(false);
	SpawnedActor->SetActorTickEnabled(false);
	SpawnedActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}



