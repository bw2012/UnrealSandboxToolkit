// Copyright blackw 2015-2020

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "VitalSystemComponent.h"
#include "Net/UnrealNetwork.h"


UVitalSystemComponent::UVitalSystemComponent() {
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = false;

	Health = 100;
	MaxHealth = 100;

	Stamina = 60;
	MaxStamina = 60;
}


void UVitalSystemComponent::BeginPlay() {
	Super::BeginPlay();
}


void UVitalSystemComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) {
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void UVitalSystemComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	DOREPLIFETIME(UVitalSystemComponent, Health);
	DOREPLIFETIME(UVitalSystemComponent, MaxHealth);
	DOREPLIFETIME(UVitalSystemComponent, Stamina);
	DOREPLIFETIME(UVitalSystemComponent, MaxStamina);
}