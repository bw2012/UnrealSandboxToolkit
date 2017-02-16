// Copyright blackw 2015-2020

#pragma once

#include "Engine.h"
#include "Components/ActorComponent.h"
#include "SandboxComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALSANDBOXTOOLKIT_API USandboxComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	virtual void CopyTo(USandboxComponent* Target) { };
};
