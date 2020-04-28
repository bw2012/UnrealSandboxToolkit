

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "SandboxPlayerController.h"
#include "SandboxCharacter.h"
#include "SandboxObject.h"
#include "ContainerComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

ASandboxPlayerController::ASandboxPlayerController() {
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CurrentInventorySlot = -1;
	bIsGameInputBlocked = false;
}

void ASandboxPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);

	// check mouse cursor
	ASandboxCharacter* Character = Cast<ASandboxCharacter>(GetCharacter());
	if (Character && !IsGameInputBlocked()) {
		if (Character->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
			bShowMouseCursor = true;
		} else if (Character->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) {
			bShowMouseCursor = false;
		}
	} else {
		bShowMouseCursor = true;
	}

	if (!IsGameInputBlocked()) {
		FHitResult Res = TracePlayerActionPoint();
		OnTracePlayerActionPoint(Res);
		if (Res.bBlockingHit) {
			AActor* SelectedActor = Res.Actor.Get();
			if (SelectedActor) {
				SelectActionObject(SelectedActor);
			}
		}
	}

	if (bMoveToMouseCursor)	{
		MoveToMouseCursor();
	}
}

void ASandboxPlayerController::SetupInputComponent() {
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("MainAction", IE_Pressed, this, &ASandboxPlayerController::OnMainActionPressedInternal);
	InputComponent->BindAction("MainAction", IE_Released, this, &ASandboxPlayerController::OnMainActionReleasedInternal);

	InputComponent->BindAction("AltAction", IE_Pressed, this, &ASandboxPlayerController::OnAltActionPressedInternal);
	InputComponent->BindAction("AltAction", IE_Released, this, &ASandboxPlayerController::OnAltActionReleasedInternal);

	// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ToggleView", IE_Pressed, this, &ASandboxPlayerController::ToggleView);
}

void ASandboxPlayerController::MoveToMouseCursor() {
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);

	if (Hit.bBlockingHit) {
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void ASandboxPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location) {
	FVector2D ScreenSpaceLocation(Location);

	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit) {
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ASandboxPlayerController::SetNewMoveDestination(const FVector DestLocation) {
	ASandboxCharacter* Character = Cast<ASandboxCharacter>(GetCharacter());

	if (Character) {
		float const Distance = FVector::Dist(DestLocation, Character->GetActorLocation());
		if (Distance > 120.0f) {
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ASandboxPlayerController::SetDestinationPressed() {
	ASandboxCharacter* Character = Cast<ASandboxCharacter>(GetCharacter());
	if (!Character) {
		return;
	}

	if (Character->GetSandboxPlayerView() != PlayerView::TOP_DOWN) {
		return;
	}

	if (Character->IsDead()) {
		return;
	}

	bMoveToMouseCursor = true;
}

void ASandboxPlayerController::SetDestinationReleased() {
	ASandboxCharacter* Character = Cast<ASandboxCharacter>(GetCharacter());
	if (!Character) {
		return;
	}

	if (Character->GetSandboxPlayerView() != PlayerView::TOP_DOWN) {
		return;
	}

	if (Character->IsDead()) {
		return;
	}

	bMoveToMouseCursor = false;
}

void ASandboxPlayerController::OnMainActionPressedInternal() {
	if (!IsGameInputBlocked()) {
		OnMainActionPressed();
	}
}

void ASandboxPlayerController::OnMainActionReleasedInternal() {
	if (!IsGameInputBlocked()) {
		OnMainActionReleased();
	}
}

void ASandboxPlayerController::OnAltActionPressedInternal() {
	if (!IsGameInputBlocked()) {
		OnAltActionPressed();
	}
}

void ASandboxPlayerController::OnAltActionReleasedInternal() {
	if (!IsGameInputBlocked()) {
		OnAltActionReleased();
	}
}

void ASandboxPlayerController::OnMainActionPressed() {

}

void ASandboxPlayerController::OnMainActionReleased() {

}

void ASandboxPlayerController::OnAltActionPressed() {

}

void ASandboxPlayerController::OnAltActionReleased() {

}

void ASandboxPlayerController::OpenCrosshairWidget() {
	ASandboxCharacter* Character = Cast<ASandboxCharacter>(GetCharacter());

	if (Character) {
		if (Character->CrosshairWidget != nullptr) {
			CrosshairWidgetInstance = CreateWidget<UUserWidget>(this, Character->CrosshairWidget);
			CrosshairWidgetInstance->AddToViewport();
		}
	}
}

void ASandboxPlayerController::CloseCrosshairWidget() {
	if (CrosshairWidgetInstance != nullptr) {
		CrosshairWidgetInstance->RemoveFromViewport();
		CrosshairWidgetInstance = nullptr;
	}
}

void ASandboxPlayerController::ToggleView() {
	if (IsGameInputBlocked()) {
		return;
	}

	ASandboxCharacter* Character = Cast<ASandboxCharacter>(GetCharacter());

	if (Character) {
		if (Character->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Character->GetActorLocation()); //abort move
			Character->InitThirdPersonView();
			bShowMouseCursor = false;
		} else if (Character->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) {
			Character->InitTopDownView();
			bShowMouseCursor = true;
		}
	} else {
		bShowMouseCursor = false;
	}
}

void ASandboxPlayerController::OnPossess(APawn* aPawn) {
	Super::OnPossess(aPawn);

	ASandboxCharacter* Character = Cast<ASandboxCharacter>(aPawn);
	if (Character) {
		Character->EnableInput(this);
		if (Character->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
			bShowMouseCursor = true;
		} else {
			bShowMouseCursor = false;
		}
	}

}

void ASandboxPlayerController::BlockGameInput() {
	//UWidgetBlueprintLibrary::SetInputMode_GameAndUI(this, nullptr, false, false);
	bIsGameInputBlocked = true;
	bShowMouseCursor = true;
}

void ASandboxPlayerController::UnblockGameInput() {
	//UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
	bIsGameInputBlocked = false;
	bShowMouseCursor = false;
}

FHitResult ASandboxPlayerController::TracePlayerActionPoint() {
	ASandboxCharacter* Character = Cast<ASandboxCharacter>(GetCharacter());
	if (!Character) {
		return FHitResult();
	}

	if (Character->GetSandboxPlayerView() == PlayerView::THIRD_PERSON || Character->GetSandboxPlayerView() == PlayerView::FIRST_PERSON) {
		float MaxUseDistance = Character->InteractionTargetLength;

		if (Character->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) {
			if (Character->GetCameraBoom() != NULL) {
				MaxUseDistance = Character->GetCameraBoom()->TargetArmLength + MaxUseDistance;
			}
		}

		FVector CamLoc;
		FRotator CamRot;
		GetPlayerViewPoint(CamLoc, CamRot);

		const FVector StartTrace = CamLoc;
		const FVector Direction = CamRot.Vector();
		const FVector EndTrace = StartTrace + (Direction * MaxUseDistance);

		FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
		//TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = false;
		TraceParams.bTraceComplex = false;
		TraceParams.AddIgnoredActor(Character);

		FHitResult Hit(ForceInit);
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams);

		return Hit;
	}

	if (Character->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Camera, false, Hit);
		return Hit;
	}

	return FHitResult();
}

void SetRenderCustomDepth(AActor* Actor, bool RenderCustomDepth) {
	TArray<UStaticMeshComponent*> MeshComponentList;
	Actor->GetComponents<UStaticMeshComponent>(MeshComponentList);

	for (UStaticMeshComponent* MeshComponent : MeshComponentList) {
		MeshComponent->SetRenderCustomDepth(RenderCustomDepth);
	}
}

void ASandboxPlayerController::SelectActionObject(AActor* Actor) {
	ASandboxObject* Obj = Cast<ASandboxObject>(Actor);

	if (SelectedObject != Obj) {
		if (SelectedObject != nullptr && SelectedObject->IsValidLowLevel()) {
			SetRenderCustomDepth(SelectedObject, false);
		}
	}

	if (Obj != nullptr) {
		SetRenderCustomDepth(Obj, true);
		SelectedObject = Obj;
	} else {
		if (SelectedObject != nullptr && SelectedObject->IsValidLowLevel()) {
			SetRenderCustomDepth(SelectedObject, false);
		}
	}
}

UContainerComponent* ASandboxPlayerController::GetInventory() {
	APawn* Pawn = GetPawn();

	if (Pawn != nullptr) {
		TArray<UContainerComponent*> Components;
		Pawn->GetComponents<UContainerComponent>(Components);

		for (UContainerComponent* Container : Components) {
			// always use only first container
			if (Container->GetName() == TEXT("Inventory")) {

			}

			return Container;
		}
	}

	return nullptr;
}

void ASandboxPlayerController::PutCurrentInventoryObjectToWorld() {
	ASandboxCharacter* Pawn = Cast<ASandboxCharacter>(GetCharacter());
	if (Pawn->IsDead()) return;

	UContainerComponent* Inventory = GetInventory();

	if (Inventory != nullptr) {
		FContainerStack* Stack = Inventory->GetSlot(CurrentInventorySlot);
		if (Stack != nullptr) {
			if (Stack->Amount > 0) {
				TSubclassOf<ASandboxObject>	ObjectClass = Stack->ObjectClass;
				ASandboxObject* Object = Stack->Object;

				if (ObjectClass != nullptr) {
					FHitResult ActionPoint = TracePlayerActionPoint();
					if (ActionPoint.bBlockingHit) {
						FVector test = ActionPoint.Location;
						test.Z = test.Z + 100;

						UClass* Cls = ObjectClass;
						FTransform Transform(FRotator(0), ActionPoint.Location, FVector(1));
						ASandboxObject* NewObject = (ASandboxObject*)GetWorld()->SpawnActor(Cls, &Transform);

						if (NewObject != nullptr) {
							Inventory->DecreaseObjectsInContainer(CurrentInventorySlot, 1);
						}

						return;
					}
				}

				if (Object != nullptr) {
					FHitResult ActionPoint = TracePlayerActionPoint();
					if (ActionPoint.bBlockingHit) {
						FVector test = ActionPoint.Location;
						test.Z = test.Z + 100;

						UClass* Cls = Object->GetClass();
						FTransform Transform(FRotator(0), ActionPoint.Location, FVector(1));
						ASandboxObject* NewObject = (ASandboxObject*)GetWorld()->SpawnActor(Cls, &Transform);

						if (NewObject != nullptr) {
							Object->CopyTo(NewObject);
							Inventory->DecreaseObjectsInContainer(CurrentInventorySlot, 1);
						}

						return;
					}
				}
			}
		}
	}
}

void ASandboxPlayerController::TakeObjectToInventory() {
	UContainerComponent* Inventory = GetInventory();

	if (Inventory != nullptr) {
		FHitResult ActionPoint = TracePlayerActionPoint();
		if (ActionPoint.bBlockingHit) {
			ASandboxObject* Obj = Cast<ASandboxObject>(ActionPoint.GetActor());
			if (Obj != nullptr) {
				if (Inventory->AddObject(Obj)) {
					Obj->Destroy();
				}
			}
		}
	}
}

bool ASandboxPlayerController::OpenObjectWithContainer() {
	FHitResult ActionPoint = TracePlayerActionPoint();

	if (ActionPoint.bBlockingHit) {
		ASandboxObject* Obj = Cast<ASandboxObject>(ActionPoint.GetActor());
		if (Obj != nullptr) {
			TArray<UContainerComponent*> Components;
			Obj->GetComponents<UContainerComponent>(Components);

			for (UContainerComponent* Container : Components) {
				if (Container->GetName().Equals(TEXT("ObjectContainer"))) {
					this->OpenedObject = Obj;
					this->OpenedContainer = Container;
					return true;
				}
			}
		}
	}

	return false;
}

void ASandboxPlayerController::CloseObjectWithContainer() {
	this->OpenedObject = nullptr;
	this->OpenedContainer = nullptr;
}


void ASandboxPlayerController::OnTracePlayerActionPoint(const FHitResult& Res) {

}


bool ASandboxPlayerController::IsGameInputBlocked() {
	ASandboxCharacter* Character = Cast<ASandboxCharacter>(GetCharacter());
	if (Character && !Character->InputEnabled()) {
		return true;
	}

	return bIsGameInputBlocked; 
}