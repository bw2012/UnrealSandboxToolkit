
#pragma once

#include "Engine.h"
#include "GameFramework/PlayerController.h"
#include "SandboxPlayerController.generated.h"


UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Sandbox")
	int32 CurrentInventorySlot;

	ASandboxPlayerController();

	virtual void Possess(APawn* aPawn) override;

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void BlockGameInput();

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	void UnblockGameInput();

	UFUNCTION(BlueprintCallable, Category = "Sandbox")
	bool IsGameInputBlocked() { return bIsGameInputBlocked; }

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
	void SetDestinationPressed();

	void SetDestinationReleased();

	// ==========================================================================================================

	virtual void OnMainActionPressed();

	virtual void OnMainActionReleased();

	virtual void OnAltActionPressed();

	virtual void OnAltActionReleased();

	virtual void ToggleView();

public:

	void ShowMouseCursor(bool bShowCursor) { this->bShowMouseCursor = bShowCursor;  };

private:

	FHitResult CerrentPos;

	bool bIsGameInputBlocked;

};


