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

	for (int i = 0; i < Content.Num(); i++) {
		FContainerStack stack = Content[i];
		if (stack.cid != 0) {
			return false;
		}
	}

	return true;
}

bool UContainerComponent::addStackToInventory(FContainerStack stack, int slot) {
	if (slot >= Content.Num()) {
		Content.SetNum(slot + 1);
	}

	FContainerStack* stack2 = &Content[slot];
	stack2->amount = stack.amount;
	stack2->cid = stack.cid;

	return true;
}


bool UContainerComponent::addItemToInventory(ASandboxObject* item) {
	bool added = false;
	for (int i = 0; i < Content.Num(); i++) {
		FContainerStack* stack = &Content[i];
		if (stack->amount != 0) {
			int item_cid = item->GetSandboxCid();
			if (item_cid == stack->cid) {
				stack->amount++;
				added = true;
				break;
			}
		} else {
			stack->cid = item->GetSandboxCid();
			stack->amount = 1; 
			added = true;
			break;
		}
	}

	if (!added) {
		FContainerStack stack;
		stack.cid = item->GetSandboxCid();
		stack.amount = 1;

		Content.Add(stack);
	}
	
	return true;
}


FContainerStack* UContainerComponent::getInventorySlot(int slot) {
	if (!Content.IsValidIndex(slot)) {
		return NULL;
	}

	return &Content[slot];
}

void UContainerComponent::DecreaseObjectsInContainer(int slot, int num) {
	FContainerStack* stack = getInventorySlot(slot);

	if (stack == NULL) {
		return;
	}

	if (stack->amount > 0) {
		stack->amount -= num;

		if (stack->amount == 0) {
			//playerInventory.Remove(slot);
			stack->cid = -1;
		}
	}
}


ASandboxObject* UContainerComponent::GetSandboxObjectFromContainer(int slot) {
	if (slot < 0) {
		return NULL;
	}

	FContainerStack* stack = getInventorySlot(slot);
	if (stack != NULL) {
		if (stack->cid > 0) {
			//UClass* cls = selectClassById(stack->cid);
			//ASandboxObject* obj = (ASandboxObject*)cls->GetDefaultObject();
			//return obj;
		}
	}

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


	if (stack1.cid == stack2.cid) {
		ASandboxObject* obj = GetSandboxObjectFromContainer(slot2);
		if (obj != NULL && obj->GetMaxStackSize() > 1) {
			FContainerStack stack_target;
			(&Content[slot2])->amount += stack1.amount;
			(&Content[slot1])->clear();
			return true;
		}
	} 
	
	
	addStackToInventory(stack1, slot2);
	addStackToInventory(stack2, slot1);
	
	return true;
}