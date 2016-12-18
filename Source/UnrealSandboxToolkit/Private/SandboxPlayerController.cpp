

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


