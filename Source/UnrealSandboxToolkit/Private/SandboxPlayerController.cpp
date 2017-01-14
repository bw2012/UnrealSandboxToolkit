

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "SandboxPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "SandboxCharacter.h"
#include "SandboxObject.h"

ASandboxPlayerController::ASandboxPlayerController() {
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
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
		CrosshairWidgetInstance = CreateWidget<UUserWidget>(this, Pawn->CrosshairWidget);
		CrosshairWidgetInstance->AddToViewport();
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

UFUNCTION(BlueprintCallable, Category = "Sandbox")
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