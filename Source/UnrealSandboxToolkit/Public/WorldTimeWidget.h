// Copyright blackw 2015-2020

#pragma once

#include "EngineMinimal.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "Blueprint/UserWidget.h"
#include "WorldTimeWidget.generated.h"


class ASandboxEnvironment;

/**
 * 
 */
UCLASS()
class UNREALSANDBOXTOOLKIT_API UWorldTimeWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Sandbox Widget")
	FString GetTimeString();
	
	UFUNCTION(BlueprintCallable, Category = "Sandbox Widget")
	FString GetLatLngString();

	UFUNCTION(BlueprintCallable, Category = "Sandbox Widget")
	FString GetDateString();


protected:

	/** Adds the widget to the screen, either to the viewport or to the player's screen depending on if the LocalPlayer is null. */
	virtual void AddToScreen(ULocalPlayer* LocalPlayer, int32 ZOrder) override;

private:
	UPROPERTY()
	ASandboxEnvironment* SandboxEnvironment;
};
