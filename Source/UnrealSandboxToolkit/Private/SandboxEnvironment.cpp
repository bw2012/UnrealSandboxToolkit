// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "SandboxEnvironment.h"
#include <ctime>
#include "SunPos.h"


ASandboxEnvironment::ASandboxEnvironment() {
	PrimaryActorTick.bCanEverTick = true;
	TimeScale = 1.f;
	TimeSpeed = 10.f;
	MaxSkyLigthIntensity = 1.0;
	NightSkyLigthIntensity = 0.01;
	LastSkyIntensity = 0;
	RecaptureSkyTreshold = 0.5f;

	bEnableDayNightCycle = true;

	Lng = 27.55;
	Lat = 53.91;

	TimeZone = +3;
}

void ASandboxEnvironment::BeginPlay() {
	Super::BeginPlay();
	
	if (DirectionalLightSource != NULL && SkySphere != NULL){
		DirectionalLightSource->SetActorRotation(FRotator(-90.0f, 0.0f, 0.0f));
	}
}

void ASandboxEnvironment::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );

	if(bEnableDayNightCycle) {
		PerformDayNightCycle();
	}
}

void ASandboxEnvironment::PerformDayNightCycle() {
	UWorld* World = GetWorld();
	AGameStateBase* GameState = World->GetGameState();

	if (GameState == NULL) {
		return;
	}

	SandboxGameTime GameTimeOfDay = ClcGameTimeOfDay(GameState->GetServerWorldTimeSeconds());

	cTime Time;
	Time.iYear = InitialYear;
	Time.iMonth = InitialMonth;
	Time.iDay = InitialDay;

	Time.dHours = GameTimeOfDay.hours + TimeZone;
	Time.dMinutes = GameTimeOfDay.minutes;
	Time.dSeconds = GameTimeOfDay.seconds;

	cLocation GeoLoc;
	GeoLoc.dLongitude = Lng;
	GeoLoc.dLatitude = Lat;

	cSunCoordinates SunPosition;

	sunpos(Time, GeoLoc, &SunPosition);

	if (DirectionalLightSource != NULL && SkySphere != NULL) {
		DirectionalLightSource->SetActorRotation(FRotator(-(90 - SunPosition.dZenithAngle), SunPosition.dAzimuth, 0.0f));

		FOutputDeviceNull Ar;
		SkySphere->CallFunctionByNameWithArguments(TEXT("UpdateSunDirection"), Ar, NULL, true);

		UFloatProperty* SunHeightFloatProp = FindField<UFloatProperty>(SkySphere->GetClass(), TEXT("Sun Height"));
		if (SunHeightFloatProp != NULL) {
			float SunHeightFloatVal = SunHeightFloatProp->GetPropertyValue_InContainer(SkySphere);

			ULightComponent* LightComponent = DirectionalLightSource->GetLightComponent();
			if (LightComponent != NULL) {
				if (SunHeightFloatVal > 0.01) { //0.08 if mobile (2.75)
					LightComponent->SetIntensity(1.75 + 2 * SunHeightFloatVal);
				}
				else {
					LightComponent->SetIntensity(0);
				}
			}

			if (SkyLight != NULL) {
				// set sky light intensity
				USkyLightComponent* SkyLightComponent = SkyLight->GetLightComponent();
				if (SkyLightComponent != NULL) {
					if (SunHeightFloatVal > 0) {
						SkyLightComponent->Intensity = 0.04 + MaxSkyLigthIntensity * SunHeightFloatVal;
					}
					else {
						SkyLightComponent->Intensity = NightSkyLigthIntensity; //night
					}

					if (FMath::Abs(SkyLightComponent->Intensity - LastSkyIntensity) > RecaptureSkyTreshold) {
						SkyLightComponent->RecaptureSky();
						LastSkyIntensity = SkyLightComponent->Intensity;
					}
				}
			}
		}
	}
}


float ASandboxEnvironment::ClcGameTime(float RealServerTime) {
	return ((RealServerTime)* TimeSpeed) + TimeOffset;
}


SandboxGameTime ASandboxEnvironment::ClcLocalGameTime(float RealServerTime) {
	long input_seconds = (long)(ClcGameTime(RealServerTime));

	const int cseconds_in_day = 86400;
	const int cseconds_in_hour = 3600;
	const int cseconds_in_minute = 60;
	const int cseconds = 1;

	SandboxGameTime ret;
	ret.days = input_seconds / cseconds_in_day;
	ret.hours = (input_seconds % cseconds_in_day) / cseconds_in_hour;
	ret.minutes = ((input_seconds % cseconds_in_day) % cseconds_in_hour) / cseconds_in_minute;
	ret.seconds = (((input_seconds % cseconds_in_day) % cseconds_in_hour) % cseconds_in_minute) / cseconds;

	return ret;
}

SandboxGameTime ASandboxEnvironment::ClcGameTimeOfDay(float RealServerTime) {
	long input_seconds = (int)(ClcGameTime(RealServerTime) + 60 * 60 * 12);

	time_t rawtime = (time_t)input_seconds;

	tm ptm;

	gmtime_s(&ptm, &rawtime);

	SandboxGameTime ret;
	ret.hours = ptm.tm_hour;
	ret.minutes = ptm.tm_min;
	ret.seconds = ptm.tm_sec;

	return ret;
}


void ASandboxEnvironment::SandboxSetTimeOffset(float Offset) {
	TimeOffset = Offset;
}

