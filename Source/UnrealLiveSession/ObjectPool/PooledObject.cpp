// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/PooledObject.h"
#include "MyObjectPool.h"

// Sets default values for this component's properties
UPooledObject::UPooledObject()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}
// Called when the game starts
void UPooledObject::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UPooledObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPooledObject::Init(class AMyObjectPool* Owner)
{
	bIsPoolActive = false;
	
	ObjectPool = Owner;
}

void UPooledObject::RecycleSelf()
{
	ObjectPool->RecyclePooledObject(this);
}

void UPooledObject::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	ObjectPool->OnPoolerCleanup.RemoveDynamic(this, &UPooledObject::RecycleSelf);
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

