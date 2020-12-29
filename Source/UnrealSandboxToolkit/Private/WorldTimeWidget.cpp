// Copyright blackw 2015-2020

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "WorldTimeWidget.h"

#include "SandboxEnvironment.h"

FString UWorldTimeWidget::GetTimeString() {
	if (SandboxEnvironment->IsValidLowLevel()) {
		AGameStateBase* GameState = GetWorld()->GetGameState();
		SandboxGameTime GameTimeOfDay = SandboxEnvironment->ClcGameTimeOfDay(GameState->GetServerWorldTimeSeconds(), true);

		return FString::Printf(TEXT("%02d:%02d"), GameTimeOfDay.hours, GameTimeOfDay.minutes);
	}

	return TEXT("");
}

FString UWorldTimeWidget::GetLatLngString() {
	if (SandboxEnvironment->IsValidLowLevel()) {
		return FString::Printf(TEXT("%f %f"), SandboxEnvironment->Lat, SandboxEnvironment->Lng);
	}

	return TEXT("");
}

FString UWorldTimeWidget::GetDateString() {
	if (SandboxEnvironment->IsValidLowLevel()) {
		AGameStateBase* GameState = GetWorld()->GetGameState();
		SandboxGameTime GameTime = SandboxEnvironment->ClcGameTimeOfDay(GameState->GetServerWorldTimeSeconds(), true);
		//return FString::Printf(TEXT("%d.%d.%d"), SandboxEnvironment->InitialDay, SandboxEnvironment->InitialMonth, SandboxEnvironment->InitialYear);
		return FString::Printf(TEXT("%d.%02d.%d"), GameTime.days, GameTime.month, GameTime.year);
	}

	return TEXT("");
}

void UWorldTimeWidget::AddToScreen(ULocalPlayer* LocalPlayer, int32 ZOrder) {
	Super::AddToScreen(LocalPlayer, ZOrder);

	for (TActorIterator<ASandboxEnvironment> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		SandboxEnvironment = *ActorItr;
		break;
	}
}