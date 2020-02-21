// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "ContainerComponent.h"
#include "SandboxComponent.h"
#include "SandboxObject.generated.h"

UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxObject : public AActor {
	GENERATED_BODY()
	
public:	
	ASandboxObject();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sandbox", meta = (AllowPrivateAccess = "true"))
	class UContainerComponent* Container;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	UStaticMeshComponent* SandboxRootMesh;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	UTexture2D* IconTexture;

	UPROPERTY(EditAnywhere, Category = "Sandbox Inventory")
	bool bStackable;

	UPROPERTY(EditAnywhere, Category = "Sandbox Inventory")
	uint32 MaxStackSize;

	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual FString GetSandboxName();

	virtual int GetSandboxCid();

	virtual int GetMaxStackSize();

	virtual bool PickUp();
	
	virtual UTexture2D* GetSandboxIconTexture();

	virtual void tickInInventoryActive(float DeltaTime, UWorld* w, FHitResult& hit);

	virtual void actionInInventoryActive(UWorld* w, FHitResult& hit);

	virtual void actionInInventoryActive2(UWorld* w, FHitResult& hit);

	virtual bool CanTake(AActor* actor);

	virtual void informTerrainChange(int32 item);
    
    virtual UContainerComponent* GetContainer(){ return Container; }

	void CopyTo(ASandboxObject* Target) {
		TArray<USandboxComponent*> TargetComponents;
		Target->GetComponents<USandboxComponent>(TargetComponents);

		for (USandboxComponent* TargetComponent : TargetComponents) {
			TArray<USandboxComponent*> SourceComponents;
			this->GetComponents<USandboxComponent>(SourceComponents);

			for (USandboxComponent* SourceComponent : SourceComponents) {
				if (SourceComponent->GetName().Equals(TargetComponent->GetName())) {
					SourceComponent->CopyTo(TargetComponent);
				}
			}
		}
	}

};
