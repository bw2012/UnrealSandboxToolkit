// Copyright blackw 2015-2020

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "ObjectContainerCellWidget.h"
#include "ContainerComponent.h"
#include "SandboxObject.h"
#include "SandboxPlayerController.h"


ESlateVisibility USandboxObjectContainerCellWidget::IsSlotSelected(int32 SlotId) {
	ASandboxPlayerController* PlayerController = Cast<ASandboxPlayerController>(GetOwningPlayer());
	if (PlayerController != NULL) {
		if (PlayerController->CurrentInventorySlot == SlotId) {
			return ESlateVisibility::Visible;
		}
	}

	return ESlateVisibility::Hidden;
}

FLinearColor USandboxObjectContainerCellWidget::SlotBorderColor(int32 SlotId) {
	return FLinearColor(0, 0, 0, 0);
}


FString USandboxObjectContainerCellWidget::SlotGetAmountText(int32 SlotId) {
	UContainerComponent* Container = GetContainer();
	if (Container != NULL) {
		FContainerStack* Stack = Container->getInventorySlot(SlotId);
		if (Stack != NULL) {
			if (Stack->Object != NULL) {
				if (Stack->amount > 0) {
					return FString::Printf(TEXT("%d"), Stack->amount);
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

	return NULL;
}

UTexture2D* USandboxObjectContainerCellWidget::GetSlotTexture(int32 SlotId) {
	UContainerComponent* Container = GetContainer();
	if (Container != NULL) {
		FContainerStack* Stack = Container->getInventorySlot(SlotId);
		if (Stack != NULL) {
			if (Stack->Object != NULL) {
				return Stack->Object->IconTexture;
			}
		}
	}

	return NULL;
}

void USandboxObjectContainerCellWidget::SelectSlot(int32 SlotId) {

}

bool USandboxObjectContainerCellWidget::SlotDrop(int32 SlotDropId, int32 SlotTargetId, AActor* SourceActor) {
	return false;
}

bool USandboxObjectContainerCellWidget::SlotIsEmpty(int32 SlotId) {
	return true;
}


AActor* USandboxObjectContainerCellWidget::GetOpenedObject() {
	return NULL;
}