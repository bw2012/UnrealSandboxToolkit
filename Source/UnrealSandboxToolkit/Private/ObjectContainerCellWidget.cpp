// Copyright blackw 2015-2020

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "ObjectContainerCellWidget.h"
#include "ContainerComponent.h"
#include "SandboxObject.h"
#include "SandboxPlayerController.h"

FLinearColor USandboxObjectContainerCellWidget::SlotBorderColor(int32 SlotId) {
	ASandboxPlayerController* PlayerController = Cast<ASandboxPlayerController>(GetOwningPlayer());
	if (PlayerController != NULL) {
		if (PlayerController->CurrentInventorySlot == SlotId) {
			return FLinearColor(0.1, 0.4, 1, 1);
		}
	}

	return FLinearColor(0, 0, 0, 0);
}


FString USandboxObjectContainerCellWidget::SlotGetAmountText(int32 SlotId) {
	UContainerComponent* Container = GetContainer();
	if (Container != NULL) {
		FContainerStack* Stack = Container->GetSlot(SlotId);
		if (Stack != NULL) {
			if (Stack->ObjectClass != NULL) {
				if (Stack->Amount > 0) {
					return FString::Printf(TEXT("%d"), Stack->Amount);
				}
			}
		}
	}

	return TEXT("");
}

UContainerComponent* USandboxObjectContainerCellWidget::GetContainer() {
	APlayerController* PlayerController = GetOwningPlayer();
	APawn* Pawn = PlayerController->GetPawn();

	TArray<UContainerComponent*> Components;
	Pawn->GetComponents<UContainerComponent>(Components);

	for (UContainerComponent* Container : Components) {
		// always use only first container
		return Container;
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
			}
		}
	}
	
	return nullptr;
}

void USandboxObjectContainerCellWidget::SelectSlot(int32 SlotId) {

}

bool USandboxObjectContainerCellWidget::SlotDrop(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor) {
	UE_LOG(LogTemp, Warning, TEXT("UI cell drop: drop id -> %d ---> target id -> %d"), SlotDropId, SlotTargetId);

	APlayerController* PlayerController = GetOwningPlayer();
	APawn* Pawn = PlayerController->GetPawn();

	if (SourceActor == nullptr) {
		UContainerComponent* Container = GetContainer();
		if (Container == NULL) {
			return false;
		}

		FContainerStack TempStack;
		FContainerStack* StackSourcePtr = Container->GetSlot(SlotDropId);
		FContainerStack* StackTargetPtr = Container->GetSlot(SlotTargetId);

		if (StackTargetPtr != NULL) {
			TempStack = *StackTargetPtr;
		}

		Container->AddStack(*StackSourcePtr, SlotTargetId);
		Container->AddStack(TempStack, SlotDropId);
	}

	return false;
}

bool USandboxObjectContainerCellWidget::SlotIsEmpty(int32 SlotId) {
	return true;
}


AActor* USandboxObjectContainerCellWidget::GetOpenedObject() {
	return NULL;
}