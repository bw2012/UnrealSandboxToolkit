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
	AStaticMeshActor* CaveSphere;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float TimeScale;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float TimeSpeed;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float MaxSkyLigthIntensity;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float NightSkyLigthIntensity;

	UPROPERTY(EditAnywhere, Category = "Sandbox")
	float RecaptureSkyTreshold;

	UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
	float MinCaveSkyLightIntensity;

	UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
	float ThresholdStartLightFalloff;

	UPROPERTY(EditAnywhere, Category = "Sandbox Cave")
	float ThresholdEndLightFalloff;

	UPROPERTY(EditAnywhere, Category = "Sandbox DayNight cycle")
	bool bEnableDayNightCycle;

	UPROPERTY(EditAnywhere, Category = "Sandbox Time")
	int InitialYear = 2016;

	UPROPERTY(EditAnywhere, Category = "Sandbox Time")
	int InitialMonth = 6;

	UPROPERTY(EditAnywhere, Category = "Sandbox Time")
	int InitialDay = 10;

	UPROPERTY(EditAnywhere, Category = "Sandbox Time")
	int InitialHour = 12;

	UPROPERTY(EditAnywhere, Category = "Sandbox Time")
	int TimeZone;

	UPROPERTY(EditAnywhere, Category = "Sandbox Time")
	float Lat;

	UPROPERTY(EditAnywhere, Category = "Sandbox Time")
	float Lng;

	float ClcGameTime(float RealServerTime);

	SandboxGameTime ClcLocalGameTime(float RealServerTime);

	SandboxGameTime ClcGameTimeOfDay(float RealServerTime, bool bAccordingTimeZone);

	void SandboxSetTimeOffset(float time);

	void UpdatePlayerPosition(FVector Pos, float GroundLevel = 0);

	void SetCaveMode(bool bCaveModeEnabled);

private:

	bool bCaveMode = false;

	float LastTime;

	float LastSkyIntensity;

	float TimeOffset = 0;

	void PerformDayNightCycle();

	float LastSunHeight;
	
};
