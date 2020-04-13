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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;




	float GetHealth();

	float GetMaxHealth();

	void ChangeHealth(float Val);

	void Damage(float DamageVal);

	float GetStamina();

	float GetMaxStamina();

	void ChangeStamina(float Val);

private:

	bool IsOwnerAdmin();

	FTimerHandle Timer;

	void PerformTimer();

};
