// Copyright blackw 2015-2020

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "SandboxObject.h"
#include "ContainerComponent.h"


// Sets default values for this component's properties
UContainerComponent::UContainerComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UContainerComponent::BeginPlay() {
	Super::BeginPlay();

}


// Called every frame
void UContainerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

bool  UContainerComponent::isEmpty() {
	if (Content.Num() == 0) {
		return true;
	}

	for (int Idx = 0; Idx < Content.Num(); Idx++) {
		FContainerStack Stack = Content[Idx];
		if (Stack.Amount > 0 ) {
			return false;
		}
	}

	return true;
}

bool UContainerComponent::AddStack(const FContainerStack Stack, const int SlotId) {
	if (SlotId >= Content.Num()) {
		Content.SetNum(SlotId + 1);
	}

	FContainerStack* StackPtr = &Content[SlotId];
	StackPtr->Amount = Stack.Amount;
	StackPtr->ObjectClass = Stack.ObjectClass;
	StackPtr->Object = Stack.Object;

	return true;
}


bool UContainerComponent::AddObject(ASandboxObject* Obj) {
	if (Obj == nullptr) {
		return false;
	}

	uint32 MaxStackSize = Obj->GetMaxStackSize();

	int FirstEmptySlot = -1;
	bool bIsAdded = false;
	for (int Idx = 0; Idx < Content.Num(); Idx++) {
		FContainerStack* Stack = &Content[Idx];

		//TODO check inventory max volume and mass
		if (Stack->Amount != 0) {
			if (Stack->ObjectClass != nullptr && MaxStackSize > 1) {
				if (Stack->ObjectClass->GetName().Equals(Obj->GetClass()->GetName())) {
					Stack->Amount++;
					bIsAdded = true;
					break;
				}
			}
		} else {
			if (FirstEmptySlot < 0) {
				FirstEmptySlot = Idx;

				if (MaxStackSize == 1) {
					break;
				}
			}
		}
	}

	if (!bIsAdded) {
		if (FirstEmptySlot >= 0) {
			FContainerStack* Stack = &Content[FirstEmptySlot];
			Stack->Amount = 1;

			if (MaxStackSize == 1) {
				Stack->Object = Obj;
			} else {
				Stack->ObjectClass = Obj->GetClass();
			}
		} else {
			FContainerStack NewStack;
			NewStack.Amount = 1;

			if (MaxStackSize == 1) {
				NewStack.Object = Obj;
			}
			else {
				NewStack.ObjectClass = Obj->GetClass();
			}

			Content.Add(NewStack);
		}
	}
	
	return true;
}


FContainerStack* UContainerComponent::GetSlot(const int Slot) {
	if (!Content.IsValidIndex(Slot)) {
		return nullptr;
	}

	return &Content[Slot];
}

void UContainerComponent::DecreaseObjectsInContainer(int Slot, int Num) {
	FContainerStack* Stack = GetSlot(Slot);

	if (Stack == NULL) {
		return;
	}

	if (Stack->Amount > 0) {
		Stack->Amount -= Num;

		if (Stack->Amount == 0) {
			Stack->Clear();
		}
	}
}


ASandboxObject* UContainerComponent::GetSandboxObjectFromContainer(int slot) {
	return NULL;
}


bool UContainerComponent::inventoryTransfer(int32 slot1, int32 slot2) {
	if (slot1 == slot2) {
		return false;
	}
	
	FContainerStack stack1;
	if (Content.IsValidIndex(slot1)) {
		stack1 = Content[slot1];
	}

	FContainerStack stack2;
	if (Content.IsValidIndex(slot2)) {
		stack2 = Content[slot2];
	}

	/*
	if (stack1.cid == stack2.cid) {
		ASandboxObject* obj = GetSandboxObjectFromContainer(slot2);
		if (obj != NULL && obj->GetMaxStackSize() > 1) {
			FContainerStack stack_target;
			(&Content[slot2])->amount += stack1.amount;
			(&Content[slot1])->clear();
			return true;
		}
	} 
	*/
	
	AddStack(stack1, slot2);
	AddStack(stack2, slot1);
	
	return true;
}