// Copyright blackw 2015-2020

#pragma once

#include "Engine.h"
#include "Components/ActorComponent.h"
#include "SandboxComponent.h"
#include "VitalSystemComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALSANDBOXTOOLKIT_API UVitalSystemComponent : public USandboxComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health")
	float Health;

	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health")
	float MaxHealth;

	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health")
	float Stamina;

	UPROPERTY(Replicated, EditAnywhere, Category = "Sandbox Health")
	float MaxStamina;

public:	
	// Sets default values for this component's properties
	UVitalSystemComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
};
