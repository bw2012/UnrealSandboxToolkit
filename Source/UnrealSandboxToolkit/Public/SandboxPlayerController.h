
#pragma once

#include "Engine.h"
#include "GameFramework/PlayerController.h"
#include "SandboxPlayerController.generated.h"


UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASandboxPlayerController();

	virtual void Possess(APawn* aPawn) override;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();

	void OnSetDestinationReleased();

	// ==========================================================================================================

	virtual void OnMainActionPressed();

	virtual void OnMainActionReleased();

	virtual void ToggleView();

	FTimerHandle timer;

public:

	void ShowMouseCursor(bool bShowCursor) { this->bShowMouseCursor = bShowCursor;  };

private:

	FHitResult CerrentPos;

};


