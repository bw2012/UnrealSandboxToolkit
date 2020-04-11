// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "SandboxObject.h"
#include "Runtime/Engine/Classes/Engine/DataAsset.h"
#include "SandboxObjectMap.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UNREALSANDBOXTOOLKIT_API USandboxObjectMap : public UDataAsset {
	GENERATED_BODY()
	
public:	
	
	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	TMap<int32, UBlueprint*> ObjectMap;


};
