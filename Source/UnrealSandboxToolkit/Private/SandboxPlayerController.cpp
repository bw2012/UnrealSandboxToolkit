

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "SandboxPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "SandboxCharacter.h"
#include "SandboxObject.h"
#include "ContainerComponent.h"

ASandboxPlayerController::ASandboxPlayerController() {
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CurrentInventorySlot = -1;
}

void ASandboxPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);

	if (bMoveToMouseCursor)	{
		MoveToMouseCursor();
	}
}

void ASandboxPlayerController::SetupInputComponent() {
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("MainAction", IE_Pressed, this, &ASandboxPlayerController::OnMainActionPressed);
	InputComponent->BindAction("MainAction", IE_Released, this, &ASandboxPlayerController::OnMainActionReleased);

	InputComponent->BindAction("AltAction", IE_Pressed, this, &ASandboxPlayerController::OnAltActionPressed);
	InputComponent->BindAction("AltAction", IE_Released, this, &ASandboxPlayerController::OnAltActionReleased);

	// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ToggleView", IE_Pressed, this, &ASandboxPlayerController::ToggleView);
}

void ASandboxPlayerController::MoveToMouseCursor() {
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_WorldStatic, false, Hit);

	if (Hit.bBlockingHit)
	{
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
	APawn* const Pawn = GetPawn();
	if (Pawn) {
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());

		if (NavSys && (Distance > 120.0f)) {
			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ASandboxPlayerController::SetDestinationPressed() {
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());
	if (pawn->GetSandboxPlayerView() != PlayerView::TOP_DOWN) return;
	if (pawn->IsDead()) return;

	bMoveToMouseCursor = true;
}

void ASandboxPlayerController::SetDestinationReleased() {
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());
	if (pawn->GetSandboxPlayerView() != PlayerView::TOP_DOWN) return;
	if (pawn->IsDead()) return;

	bMoveToMouseCursor = false;
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
	ASandboxCharacter* Pawn = Cast<ASandboxCharacter>(GetCharacter());

	if (Pawn != nullptr) {
		if (Pawn->CrosshairWidget != nullptr) {
			CrosshairWidgetInstance = CreateWidget<UUserWidget>(this, Pawn->CrosshairWidget);
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
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());

	if (pawn->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
		pawn->InitThirdPersonView();
		bShowMouseCursor = false;
	} else if (pawn->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) {
		pawn->InitTopDownView();
		bShowMouseCursor = true;
	} 
}

void ASandboxPlayerController::Possess(APawn* aPawn) {
	Super::Possess(aPawn);

	ASandboxCharacter* Pawn = Cast<ASandboxCharacter>(aPawn);
	if (Pawn != NULL) {
		if (Pawn->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
			bShowMouseCursor = true;
		} else {
			bShowMouseCursor = false;
		}
	}

}

void ASandboxPlayerController::BlockGameInput() {
	bIsGameInputBlocked = true;
	bShowMouseCursor = true;
}

void ASandboxPlayerController::UnblockGameInput() {
	bIsGameInputBlocked = false;
	bShowMouseCursor = false;
}

FHitResult ASandboxPlayerController::TracePlayerActionPoint() {
	ASandboxCharacter* Pawn = Cast<ASandboxCharacter>(GetCharacter());

	if (Pawn->GetSandboxPlayerView() == PlayerView::THIRD_PERSON || Pawn->GetSandboxPlayerView() == PlayerView::FIRST_PERSON) {
		float MaxUseDistance = Pawn->InteractionTargetLength;

		if (Pawn->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) {
			if (Pawn->GetCameraBoom() != NULL) {
				MaxUseDistance = Pawn->GetCameraBoom()->TargetArmLength + MaxUseDistance;
			}
		}

		FVector CamLoc;
		FRotator CamRot;
		GetPlayerViewPoint(CamLoc, CamRot);

		const FVector StartTrace = CamLoc;
		const FVector Direction = CamRot.Vector();
		const FVector EndTrace = StartTrace + (Direction * MaxUseDistance);

		FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
		TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = false;
		TraceParams.bTraceComplex = true;
		TraceParams.AddIgnoredActor(Pawn);

		FHitResult Hit(ForceInit);
		GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, TraceParams);

		return Hit;
	}

	if (Pawn->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);
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