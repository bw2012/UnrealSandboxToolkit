// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "SandboxObject.h"

ASandboxObject::ASandboxObject() {
	PrimaryActorTick.bCanEverTick = true;
	SandboxRootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SandboxRootMesh"));
	MaxStackSize = 100;
	bStackable = true;
}

void ASandboxObject::BeginPlay() {
	Super::BeginPlay();
}

void ASandboxObject::BeginDestroy() {
	//sandboxRemoveObject(this);
	//z_cut_object_list.Remove(this);
	Super::BeginDestroy();
}

void ASandboxObject::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );
}

static const FString DefaultSandboxObjectName = FString(TEXT("Sandbox object"));

FString ASandboxObject::GetSandboxName() {
	return DefaultSandboxObjectName;
}

int ASandboxObject::GetSandboxClassId() {
	return 0; // empty object
}

uint64 ASandboxObject::GetSandboxTypeId() {
	return 0; // default type
}

int ASandboxObject::GetMaxStackSize() {
	if (!bStackable) {
		return 1;
	}

	TArray<UContainerComponent*> Components;
	this->GetComponents<UContainerComponent>(Components);

	for (UContainerComponent* Container : Components) {
			return 1;
	}

	return 100; // empty object
}

bool ASandboxObject::PickUp() {
	USceneComponent* RootMesh = GetRootComponent();
	if (RootMesh != NULL){
		RootMesh->DestroyComponent();
	}
    
	return true; 
}

UTexture2D* ASandboxObject::GetSandboxIconTexture() {
	return NULL; 
}

void ASandboxObject::TickInInventoryActive(float DeltaTime, UWorld* World, const FHitResult& HitResult) {

}

void ASandboxObject::ActionInInventoryActive(UWorld* World, const FHitResult& HitResult) {

}

void ASandboxObject::ActionInInventoryActive2(UWorld* World, const FHitResult& HitResult) {

}

bool ASandboxObject::CanTake(AActor* actor) {
	return false; 
}

void ASandboxObject::informTerrainChange(int32 item) {

}