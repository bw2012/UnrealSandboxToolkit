// Copyright blackw 2015-2020

#pragma once

#include "Engine.h"
#include "Components/ActorComponent.h"
#include "ContainerComponent.generated.h"


class ASandboxObject;

USTRUCT()
struct FContainerStack {
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 cid;

	UPROPERTY()
	int32 amount;

	FContainerStack() {
		clear();
	}

	void clear() {
		cid = 0;
		amount = 0;
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALSANDBOXTOOLKIT_API UContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UContainerComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	TArray<FContainerStack> Content;
    
    bool addStackToInventory(FContainerStack stack, int slot);
	
	bool addItemToInventory(ASandboxObject* item);
    
    FContainerStack* getInventorySlot(int slot);
    
    void DecreaseObjectsInContainer(int slot, int num);
    
    ASandboxObject* GetSandboxObjectFromContainer(int slot);
    
    bool inventoryTransfer(int32 slot1, int32 slot2);

	bool isEmpty();
};
