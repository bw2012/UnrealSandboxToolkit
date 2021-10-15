// Copyright blackw 2015-2020

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "ObjectContainerCellWidget.h"
#include "ContainerComponent.h"
#include "SandboxObject.h"
#include "SandboxPlayerController.h"

FLinearColor USandboxObjectContainerCellWidget::SlotBorderColor(int32 SlotId) {
	if (ContainerId == 0) {
		ASandboxPlayerController* PlayerController = Cast<ASandboxPlayerController>(GetOwningPlayer());
		if (PlayerController) {
			if (PlayerController->CurrentInventorySlot == SlotId) {
				return FLinearColor(0.1, 0.4, 1, 1);
			}
		}
	}

	/*
	UContainerComponent* Container = GetContainer();
	if (Container != NULL) {
		FContainerStack* Stack = Container->GetSlot(SlotId);
		if (Stack != NULL) {
			if (Stack->Object != nullptr) {
				if (Stack->Amount > 0) {
					return FLinearColor(0.97, 1, 0.4, 1);
				}
			}
		}
	}
	*/

	return FLinearColor(0, 0, 0, 0.5);
}


FString USandboxObjectContainerCellWidget::SlotGetAmountText(int32 SlotId) {
	UContainerComponent* Container = GetContainer();
	if (Container != NULL) {
		FContainerStack* Stack = Container->GetSlot(SlotId);
		if (Stack != NULL) {
			if (Stack->ObjectClass != nullptr || Stack->Object != nullptr) {
				if (Stack->Amount > 0) {
					return FString::Printf(TEXT("%d"), Stack->Amount);
				}
			}
		}
	}

	return TEXT("");
}

UContainerComponent* USandboxObjectContainerCellWidget::GetContainer() {
	if (ContainerId == 0) {
		APawn* Pawn = GetOwningPlayer()->GetPawn();
		if (Pawn) {
			TArray<UContainerComponent*> Components;
			Pawn->GetComponents<UContainerComponent>(Components);

			for (UContainerComponent* Container : Components) {
				if (Container->GetName().Equals(TEXT("Inventory"))) {
					return Container;
				}
			}
		}
	}

	if (ContainerId == 100) { // opened object
		ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer());
		if (SandboxPC) {
			return SandboxPC->GetOpenedContainer();
		}
	}

	return nullptr;
}

UTexture2D* USandboxObjectContainerCellWidget::GetSlotTexture(int32 SlotId) {
	
	UContainerComponent* Container = GetContainer();
	if (Container != nullptr) {
		FContainerStack* Stack = Container->GetSlot(SlotId);
		if (Stack != nullptr) {
			if (Stack->Amount > 0) {
				if (Stack->ObjectClass != nullptr) {
					ASandboxObject* DefaultObject = Cast<ASandboxObject>(Stack->ObjectClass->GetDefaultObject());
					if (DefaultObject != nullptr) {
						return DefaultObject->IconTexture;
					}
				}

				if (Stack->Object != nullptr) {
					ASandboxObject* Object = Cast<ASandboxObject>(Stack->Object);
					if (Object != nullptr) {
						return Object->IconTexture;
					}
				}
			}
		}
	}
	
	return nullptr;
}

void USandboxObjectContainerCellWidget::SelectSlot(int32 SlotId) {
	UE_LOG(LogTemp, Log, TEXT("SelectSlot: %d"), SlotId);
}

bool USandboxObjectContainerCellWidget::SlotDrop(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor, UContainerComponent* SourceContainer) {
	UE_LOG(LogTemp, Log, TEXT("UI cell drop: drop id -> %d ---> target id -> %d"), SlotDropId, SlotTargetId);

	if (ContainerId == 0 && SourceActor == nullptr) { // internal transfer
		UContainerComponent* Container = GetContainer();
		if (Container == NULL) {
			return false;
		}

		if (SlotDropId == SlotTargetId) {
			return false;
		}

		FContainerStack* StackSourcePtr = Container->GetSlot(SlotDropId);
		FContainerStack* StackTargetPtr = Container->GetSlot(SlotTargetId);

		FContainerStack StackSource;
		FContainerStack StackTarget;

		if (StackTargetPtr != NULL) {
			StackTarget = *StackTargetPtr;
		}

		if (StackSourcePtr != NULL) {
			StackSource = *StackSourcePtr;
		}

		if (StackTarget.ObjectClass != nullptr && StackSource.ObjectClass != nullptr) {
			if (StackTarget.ObjectClass->GetName().Equals(StackSource.ObjectClass->GetName())) {
				StackTargetPtr->Amount += StackSourcePtr->Amount;
				StackSourcePtr->Clear();

				return true;
			}
		}

		Container->AddStack(StackSource, SlotTargetId);
		Container->AddStack(StackTarget, SlotDropId);

		return true;
	} else {
		if (SourceContainer == nullptr) {
			return false;
		}

		UContainerComponent* TargetContainer = GetContainer();

		FContainerStack* StackSourcePtr = SourceContainer->GetSlot(SlotDropId);
		FContainerStack* StackTargetPtr = TargetContainer->GetSlot(SlotTargetId);

		FContainerStack StackSource;
		FContainerStack StackTarget;

		if (StackTargetPtr != NULL) {
			StackTarget = *StackTargetPtr;
		}

		if (StackSourcePtr != NULL) {
			StackSource = *StackSourcePtr;
		}

		if (StackTarget.ObjectClass != nullptr && StackSource.ObjectClass != nullptr) {
			if (StackTarget.ObjectClass->GetName().Equals(StackSource.ObjectClass->GetName())) {
				StackTargetPtr->Amount += StackSourcePtr->Amount;
				StackSourcePtr->Clear();

				return true;
			}
		}

		SourceContainer->AddStack(StackTarget, SlotDropId);
		TargetContainer->AddStack(StackSource, SlotTargetId);
	}
	
	return false;
}

bool USandboxObjectContainerCellWidget::SlotIsEmpty(int32 SlotId) {
	return false;
}


AActor* USandboxObjectContainerCellWidget::GetOpenedObject() {
	if (ContainerId == 100) { 
		ASandboxPlayerController* SandboxPC = Cast<ASandboxPlayerController>(GetOwningPlayer());
		if (SandboxPC != nullptr) {
			return SandboxPC->GetOpenedObject();
		}
	}

	if (ContainerId == 0) {
		return GetOwningPlayer()->GetPawn();
	}

	return nullptr;
}

UContainerComponent* USandboxObjectContainerCellWidget::GetOpenedContainer() {
	return GetContainer();
}