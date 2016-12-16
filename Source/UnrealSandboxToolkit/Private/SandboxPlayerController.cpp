

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "SandboxPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "SandboxCharacter.h"

ASandboxPlayerController::ASandboxPlayerController() {
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
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

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ASandboxPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ASandboxPlayerController::OnSetDestinationReleased);

	// support touch devices 
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);
	//InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AUE4VoxelTerrainPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("Test", IE_Pressed, this, &ASandboxPlayerController::OnMainActionPressed);
	InputComponent->BindAction("Test", IE_Released, this, &ASandboxPlayerController::OnMainActionReleased);

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

void ASandboxPlayerController::OnSetDestinationPressed() {
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());
	if (pawn->GetSandboxPlayerView() != PlayerView::TOP_DOWN) {
		return;
	}

	bMoveToMouseCursor = true;
}

void ASandboxPlayerController::OnSetDestinationReleased() {
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());
	if (pawn->GetSandboxPlayerView() != PlayerView::TOP_DOWN) {
		return;
	}

	bMoveToMouseCursor = false;
}


void ASandboxPlayerController::OnMainActionReleased() {
	GetWorld()->GetTimerManager().ClearTimer(timer);
}


void ASandboxPlayerController::OnMainActionPressed() {
	
}

void ASandboxPlayerController::ToggleView() {
	ASandboxCharacter* pawn = Cast<ASandboxCharacter>(GetCharacter());

	if (pawn->GetSandboxPlayerView() == PlayerView::TOP_DOWN) {
		pawn->InitThirdPersonView();
		bShowMouseCursor = false;

		//hud->openTpvHud();
		//disableZCut();
	} else if (pawn->GetSandboxPlayerView() == PlayerView::THIRD_PERSON) {
		pawn->InitTopDownView();
		bShowMouseCursor = true;

		//hud->closeTpvHud();
		//z_cut_context.force_check = true;
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


