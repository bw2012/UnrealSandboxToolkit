// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "SandboxObject.h"

ASandboxObject::ASandboxObject() {
	PrimaryActorTick.bCanEverTick = true;
	SandboxRootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SandboxRootMesh"));
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

static const FString sandbox_object_name = FString(TEXT("test object"));

FString ASandboxObject::GetSandboxName() {
	return sandbox_object_name;
}

int ASandboxObject::GetSandboxCid() {
	return 0; // empty object
}

int ASandboxObject::GetMaxStackSize() {
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

void ASandboxObject::tickInInventoryActive(float DeltaTime, UWorld* w, FHitResult& hit) {

}

void ASandboxObject::actionInInventoryActive(UWorld* w, FHitResult& hit) {

}

void ASandboxObject::actionInInventoryActive2(UWorld* w, FHitResult& hit) {

}

bool ASandboxObject::CanTake(AActor* actor) {
	return false; 
}

void ASandboxObject::informTerrainChange(int32 item) {

}