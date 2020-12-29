// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "SandboxEnvironment.h"
#include <ctime>
#include "SunPos.h"


ASandboxEnvironment::ASandboxEnvironment() {
	PrimaryActorTick.bCanEverTick = true;
	TimeSpeed = 10.f;
	MaxDaySkyLigthIntensity = 1.0;
	MinNightSkyLigthIntensity = 0.01;
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

	if (DirectionalLightSource){
		DirectionalLightSource->SetActorRotation(FRotator(-90.0f, 0.0f, 0.0f));
	}

	if (CaveSphere) {
		CaveSphere->GetStaticMeshComponent()->SetVisibility(bCaveMode);
	}

	if (GlobalFog) {
		UExponentialHeightFogComponent* FogCmoponent = GlobalFog->GetComponent();
		FogMaxOpacity = FogCmoponent->FogMaxOpacity;
		FogMaxDensity = FogCmoponent->FogDensity;
	}

	MaxSkyLigthIntensity = MaxDaySkyLigthIntensity;
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
		if (SkyLightComponent) {
			SkyLightComponent->Intensity = Intensity;
			SkyLightComponent->RecaptureSky();
		}
	}
}

void ASandboxEnvironment::PerformDayNightCycle() {
	UWorld* World = GetWorld();
	AGameStateBase* GameState = World->GetGameState();

	if (!GameState) {
		return;
	}

	float RealServerTime = GameState->GetServerWorldTimeSeconds();
	SandboxGameTime GameDayTime = ClcGameTimeOfDay(RealServerTime, false); // use UTC time

	//UE_LOG(LogTemp, Warning, TEXT("%f"), RealServerTime);
	//UE_LOG(LogTemp, Warning, TEXT("%d : %d"), GameTimeOfDay.hours, GameTimeOfDay.minutes);

	cTime Time;
	Time.iYear = GameDayTime.year;
	Time.iMonth = GameDayTime.month;
	Time.iDay = GameDayTime.days;

	Time.dHours = GameDayTime.hours;
	Time.dMinutes = GameDayTime.minutes;
	Time.dSeconds = GameDayTime.seconds;

	cLocation GeoLoc;
	GeoLoc.dLongitude = Lng;
	GeoLoc.dLatitude = Lat;

	cSunCoordinates SunPosition;

	sunpos(Time, GeoLoc, &SunPosition);

	if (DirectionalLightSource) {
		DirectionalLightSource->SetActorRotation(FRotator(-(90 - SunPosition.dZenithAngle), SunPosition.dAzimuth, 0.0f));

		if (SkyLight) {
			float H = 1 - SunPosition.dZenithAngle / 180;
			bIsNight = H < 0.5;
			float Temp = H;

			if (Temp < 0.5f) {
				Temp /= 3;
			}

			//UE_LOG(LogTemp, Log, TEXT("dZenithAngle -> %f %f"), SunPosition.dZenithAngle, Temp);

			float Intensity = MinNightSkyLigthIntensity + Temp * MaxSkyLigthIntensity;
			SetSkyLightIntensity(SkyLight, Intensity);

			if (GlobalFog) {
				UExponentialHeightFogComponent* FogCmoponent = GlobalFog->GetComponent();
				float FogOpacity = FogMaxOpacity * Temp;

				if (GlobalFogDensityCurve) {
					float K = GlobalFogDensityCurve->GetFloatValue(H - 0.5);
					FogCmoponent->SetFogDensity(FogMaxDensity * K);
				} else {
					FogCmoponent->SetFogDensity(FogMaxDensity * Temp);
				}

				if (GlobalFogOpacityCurve) {
					float K = GlobalFogOpacityCurve->GetFloatValue(H - 0.5);
					FogCmoponent->SetFogMaxOpacity(FogMaxOpacity * K);
				} else {
					FogCmoponent->SetFogMaxOpacity(FogMaxOpacity * Temp);
				}
			}

		}
	}
}

float ASandboxEnvironment::ClcGameTime(float RealServerTime) {
	return (RealServerTime + RealTimeOffset) * TimeSpeed;
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
	std::tm initial_ptm {};
	initial_ptm.tm_hour = 12;
	initial_ptm.tm_min = 0;
	initial_ptm.tm_sec = 0;
	initial_ptm.tm_mon = InitialMonth + 1;
	initial_ptm.tm_mday = InitialDay;
	initial_ptm.tm_year = InitialYear - 1900;

	time_t initial_time = std::mktime(&initial_ptm);

	//static const uint64 InitialOffset = 60 * 60 * 12; // always start game at 12:00
	const uint64 InitialOffset = initial_time;
	const uint64 TimezoneOffset = bAccordingTimeZone ? 60 * 60 * TimeZone : 0;
	const uint64 input_seconds = (int)ClcGameTime(RealServerTime) + InitialOffset + TimezoneOffset;

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
	Time.days = ptm.tm_mday;
	Time.month = ptm.tm_mon + 1;
	Time.year = ptm.tm_year + 1900;

	return Time;
}

void ASandboxEnvironment::SetTimeOffset(float Offset) {
	RealTimeOffset = Offset;
}

double ASandboxEnvironment::GetNewTimeOffset() {
	AGameStateBase* GameState = GetWorld()->GetGameState();

	if (!GameState) {
		return RealTimeOffset;
	}

	float RealServerTime = GameState->GetServerWorldTimeSeconds();
	return RealTimeOffset + RealServerTime;
}

void ASandboxEnvironment::UpdatePlayerPosition(FVector Pos, float GroundLevel) {
	if (bCaveMode) {
		CaveSphere->SetActorLocation(Pos);
	}

	if (Pos.Z < ThresholdEndLightFalloff) {
		//SetSkyLightIntensity(SkyLight, MinCaveSkyLightIntensity);
		//MaxSkyLigthIntensity = MinCaveSkyLightIntensity;
	} else if (Pos.Z < ThresholdStartLightFalloff) {
		float H = ThresholdStartLightFalloff - Pos.Z;
		float IntensityStep = (1 - MinCaveSkyLightIntensity) / (ThresholdStartLightFalloff - ThresholdEndLightFalloff);
		float Intensity = 1 - H * IntensityStep;
		//UE_LOG(LogTemp, Log, TEXT("Intensity -> %f %f"), H, Intensity);
		//SetSkyLightIntensity(SkyLight, Intensity);
		//MaxSkyLigthIntensity = Intensity;
	} else {
		//MaxSkyLigthIntensity = MaxDaySkyLigthIntensity;
	}

	//PerformDayNightCycle();

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

bool ASandboxEnvironment::IsNight() const {
	return bIsNight;
}
