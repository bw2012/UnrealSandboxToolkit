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
	LastSunHeight = -1;
	MinCaveSkyLightIntensity = 0.5;
	ThresholdStartLightFalloff = -500;
	ThresholdEndLightFalloff = -2000;
}

void ASandboxEnvironment::BeginPlay() {
	Super::BeginPlay();

	if (DirectionalLightSource != NULL && SkySphere != NULL){
		DirectionalLightSource->SetActorRotation(FRotator(-90.0f, 0.0f, 0.0f));
	}

	if (CaveSphere) {
		CaveSphere->GetStaticMeshComponent()->SetVisibility(bCaveMode);
	}
}

void ASandboxEnvironment::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );

	if(bEnableDayNightCycle) {
		PerformDayNightCycle();
	}
}

void SetSkyLightIntensity(ASkyLight* SkyLight, float Intensity) {
	if (SkyLight) {
		USkyLightComponent* SkyLightComponent = SkyLight->GetLightComponent();
		if (SkyLightComponent != NULL) {
			SkyLightComponent->Intensity = Intensity;
		}
	}
}

void ASandboxEnvironment::PerformDayNightCycle() {
	UWorld* World = GetWorld();
	AGameStateBase* GameState = World->GetGameState();

	if (GameState == NULL) {
		return;
	}

	SandboxGameTime GameTimeOfDay = ClcGameTimeOfDay(GameState->GetServerWorldTimeSeconds(), false); // use UTC time

	//sUE_LOG(LogTemp, Warning, TEXT("%d : %d"), GameTimeOfDay.hours, GameTimeOfDay.minutes);

	cTime Time;
	Time.iYear = InitialYear;
	Time.iMonth = InitialMonth;
	Time.iDay = InitialDay;

	Time.dHours = GameTimeOfDay.hours;
	Time.dMinutes = GameTimeOfDay.minutes;
	Time.dSeconds = GameTimeOfDay.seconds;

	cLocation GeoLoc;
	GeoLoc.dLongitude = Lng;
	GeoLoc.dLatitude = Lat;

	cSunCoordinates SunPosition;

	sunpos(Time, GeoLoc, &SunPosition);

	if (DirectionalLightSource != NULL) {
		DirectionalLightSource->SetActorRotation(FRotator(-(90 - SunPosition.dZenithAngle), SunPosition.dAzimuth, 0.0f));

		if (SkySphere != NULL) {
			FOutputDeviceNull Ar;
			SkySphere->CallFunctionByNameWithArguments(TEXT("UpdateSunDirection"), Ar, NULL, true);

			UFloatProperty* SunHeightFloatProp = FindField<UFloatProperty>(SkySphere->GetClass(), TEXT("Sun Height"));
			if (SunHeightFloatProp != NULL) {
				float SunHeight = SunHeightFloatProp->GetPropertyValue_InContainer(SkySphere);

				if (LastSunHeight < 0) {
					LastSunHeight = SunHeight;
				}

				ULightComponent* LightComponent = DirectionalLightSource->GetLightComponent();
				if (LightComponent != NULL) {
					if (SunHeight > 0.01) { //0.08 if mobile (2.75)
						LightComponent->SetIntensity(1.75 + 2 * SunHeight);
					} else {
						LightComponent->SetIntensity(0);
					}
				}

				if (SkyLight != NULL) {
					// set sky light intensity
					USkyLightComponent* SkyLightComponent = SkyLight->GetLightComponent();
					if (SkyLightComponent != NULL) {
						if (SunHeight > 0) {
							SkyLightComponent->Intensity = 0.04 + MaxSkyLigthIntensity * SunHeight;
						} else {
							SkyLightComponent->Intensity = NightSkyLigthIntensity; //night
						}

						if (FMath::Abs(SkyLightComponent->Intensity - LastSkyIntensity) > RecaptureSkyTreshold) {
							SkyLightComponent->RecaptureSky();
							LastSkyIntensity = SkyLightComponent->Intensity;
						}

						if ((LastSunHeight > 0 && SunHeight < 0) || (LastSunHeight < 0 && SunHeight > 0)) {
							SkyLightComponent->RecaptureSky();
						}
					}
				}

				LastSunHeight = SunHeight;
			}
		} else {
			if (SkyLight != NULL) {
				USkyLightComponent* SkyLightComponent = SkyLight->GetLightComponent();
				if (SkyLightComponent != NULL) {
					SkyLightComponent->RecaptureSky();
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

SandboxGameTime ASandboxEnvironment::ClcGameTimeOfDay(float RealServerTime, bool bAccordingTimeZone) {
	static const long InitialOffset = 60 * 60 * 12; // always start game at 12:00
	long TimezoneOffset = bAccordingTimeZone ? 60 * 60 * TimeZone : 0;
	long input_seconds = (int)(ClcGameTime(RealServerTime) + InitialOffset + TimezoneOffset);
	time_t rawtime = (time_t)input_seconds;
	tm ptm;

#ifdef _MSC_VER
	gmtime_s(&ptm, &rawtime);
#else
	ptm = *gmtime_r(&rawtime, &ptm);
#endif

	SandboxGameTime Time;
	Time.hours = ptm.tm_hour;
	Time.minutes = ptm.tm_min;
	Time.seconds = ptm.tm_sec;
	return Time;
}

void ASandboxEnvironment::SandboxSetTimeOffset(float Offset) {
	TimeOffset = Offset;
}

void ASandboxEnvironment::UpdatePlayerPosition(FVector Pos, float GroundLevel) {
	if (bCaveMode) {
		CaveSphere->SetActorLocation(Pos);
	}

	if (Pos.Z < ThresholdEndLightFalloff) {
		SetSkyLightIntensity(SkyLight, MinCaveSkyLightIntensity);
	} else if (Pos.Z < ThresholdStartLightFalloff) {
		float H = ThresholdStartLightFalloff - Pos.Z;
		float IntensityStep = (1 - MinCaveSkyLightIntensity) / (ThresholdStartLightFalloff - ThresholdEndLightFalloff);
		float Intensity = 1 - H * IntensityStep;
		//UE_LOG(LogTemp, Log, TEXT("Intensity -> %f %f"), H, Intensity);
		SetSkyLightIntensity(SkyLight, Intensity);
	} else {
		SetSkyLightIntensity(SkyLight, 1.f);
	}

	if (AmbientSound) {
		float Value = 0;
		if (Pos.Z < -1000) {
			Value = Pos.Z / -3000;
			if (Value > 1) {
				Value = 1;
			}
		}

		FAudioComponentParam Param(TEXT("Z"));
		Param.FloatParam = Value;
		AmbientSound->GetAudioComponent()->SetSoundParameter(Param);
	}
}


void ASandboxEnvironment::SetCaveMode(bool bCaveModeEnabled) {
	if (bCaveMode == bCaveModeEnabled) {
		return;
	}

	if (CaveSphere) {
		CaveSphere->GetStaticMeshComponent()->SetVisibility(bCaveModeEnabled);
	}

	bCaveMode = bCaveModeEnabled;
}

