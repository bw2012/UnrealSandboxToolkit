// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "SandboxEnvironment.h"
#include <ctime>


ASandboxEnvironment::ASandboxEnvironment() {
	PrimaryActorTick.bCanEverTick = true;
	TimeScale = 1.f;
	MaxSkyLigthIntensity = 1.0;
	NightSkyLigthIntensity = 0.01;
	last_sky_intensity = 0;
	RecaptureSkyTreshold = 0.5f;
}

void ASandboxEnvironment::BeginPlay() {
	Super::BeginPlay();
	
	if (DirectionalLightSource != NULL && SkySphere != NULL){
		DirectionalLightSource->SetActorRotation(FRotator(-90.0f, 0.0f, 0.0f));
	}
}

void ASandboxEnvironment::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );

	UWorld* world = GetWorld();
	AGameState* gameState = world->GetGameState();

	if (gameState == NULL) {
		return;
	}

	float local_time = ClcGameTime(gameState->GetServerWorldTimeSeconds()) * TimeScale;

	float delta = local_time - last_time;
	last_time = local_time;

	float offset = (180.0f / (60.0f * 60.0f * 12.0f)) * delta;

	if (DirectionalLightSource != NULL && SkySphere != NULL) {
		FRotator myRotationValue = FRotator(0.0f, offset, 0.0f); //yaw
		DirectionalLightSource->AddActorLocalRotation(myRotationValue);

		FOutputDeviceNull ar;
		SkySphere->CallFunctionByNameWithArguments(TEXT("UpdateSunDirection"), ar, NULL, true);

		UFloatProperty* sunHeightFloatProp = FindField<UFloatProperty>(SkySphere->GetClass(), TEXT("Sun Height"));
		if (sunHeightFloatProp != NULL) {
			float sunHeightFloatVal = sunHeightFloatProp->GetPropertyValue_InContainer(SkySphere);

			ULightComponent* l = DirectionalLightSource->GetLightComponent();
			if (l != NULL) {
				if (sunHeightFloatVal > 0.01) { //0.08 if mobile
												// 2.75
					l->SetIntensity(1.75 + 2 * sunHeightFloatVal);
				}
				else {
					l->SetIntensity(0);
				}
			}

			if (SkyLight != NULL) {
				// set sky light intensity
				USkyLightComponent* slc = SkyLight->GetLightComponent();
				if (slc != NULL) {
					if (sunHeightFloatVal > 0) {
						slc->Intensity = 0.04 + MaxSkyLigthIntensity * sunHeightFloatVal;
					}
					else {
						slc->Intensity = NightSkyLigthIntensity; //night
					}

					if (FMath::Abs(slc->Intensity - last_sky_intensity) > RecaptureSkyTreshold) {
						slc->RecaptureSky();
						last_sky_intensity = slc->Intensity;
					}
				}
			}



		}
	}
}


float ASandboxEnvironment::ClcGameTime(float RealServerTime) {
	return ((RealServerTime)* 10.0f) + time_offset;
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

	return ret;
}


void ASandboxEnvironment::SandboxSetTimeOffset(float Offset) {
	time_offset = Offset;
}

