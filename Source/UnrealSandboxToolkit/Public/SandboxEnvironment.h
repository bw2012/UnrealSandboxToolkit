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
	ADirectionalLight* directionalLightSource;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	AActor* skySphere;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	ASkyLight* skyLight;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float TimeScale;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float MaxSkyLigthIntensity;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float NightSkyLigthIntensity;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float RecaptureSkyTreshold;

	float clcGameTime(float real_server_time);

	SandboxGameTime clcLocalGameTime(float real_server_time);

	SandboxGameTime clcGameTimeOfDay(float real_server_time);

	void SandboxSetTimeOffset(float time);

private:
	float last_time;

	float last_sky_intensity;

	float time_offset = 0;
	
};
