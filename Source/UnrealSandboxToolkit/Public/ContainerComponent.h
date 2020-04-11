// Copyright blackw 2015-2020

#pragma once

#include "Engine.h"
#include "Components/ActorComponent.h"
#include "SandboxComponent.h"
#include "ContainerComponent.generated.h"

class ASandboxObject;

USTRUCT()
struct FContainerStack {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	int32 Amount;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASandboxObject>	ObjectClass;

	UPROPERTY(BlueprintInternalUseOnly)
	ASandboxObject* Object;

	FContainerStack() {
		Clear();
	}

	void Clear() {
		Amount = 0;
		ObjectClass = nullptr;
		Object = nullptr;
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALSANDBOXTOOLKIT_API UContainerComponent : public USandboxComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UContainerComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox")
	TArray<FContainerStack> Content;
    
	bool AddStack(const FContainerStack Stack, const int SlotId);

	bool AddObject(ASandboxObject* Obj);
    
	FContainerStack* GetSlot(const int Slot);
    
    void DecreaseObjectsInContainer(int slot, int num);
    
    ASandboxObject* GetSandboxObjectFromContainer(int slot);
    
    bool inventoryTransfer(int32 slot1, int32 slot2);

	bool isEmpty();

	void CopyTo(USandboxComponent* Target) {
		UContainerComponent* Trg = Cast<UContainerComponent>(Target);
		if (Trg != nullptr) {
			Trg->Content = this->Content;
		}
	}

private:
	bool IsOwnerAdmin();

};
