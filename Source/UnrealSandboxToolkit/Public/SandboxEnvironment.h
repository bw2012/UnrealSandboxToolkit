// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "SandboxEnvironment.generated.h"

struct SandboxGameTime {
	long days;
	long hours;
	long minutes;
	long seconds;
};


UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxEnvironment : public AActor
{
	GENERATED_BODY()
	
public:	
	ASandboxEnvironment();

	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	ADirectionalLight* DirectionalLightSource;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	AActor* SkySphere;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	ASkyLight* SkyLight;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float TimeScale;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float MaxSkyLigthIntensity;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float NightSkyLigthIntensity;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float RecaptureSkyTreshold;

	float ClcGameTime(float RealServerTime);

	SandboxGameTime ClcLocalGameTime(float RealServerTime);

	SandboxGameTime ClcGameTimeOfDay(float RealServerTime);

	void SandboxSetTimeOffset(float time);

private:
	float LastTime;

	float LastSkyIntensity;

	float TimeOffset = 0;
	
};
