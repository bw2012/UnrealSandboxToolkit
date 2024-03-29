#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "SandboxObjectMap.h"
#include "SandboxLevelController.generated.h"


UCLASS()
class UNREALSANDBOXTOOLKIT_API ASandboxLevelController : public AActor {
	GENERATED_BODY()

public:
	ASandboxLevelController();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	FString MapName;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	USandboxObjectMap* ObjectMap;

	UPROPERTY(EditAnywhere, Category = "UnrealSandbox Toolkit")
	bool bSaveOnEndPlay;

	TSubclassOf<ASandboxObject> GetSandboxObjectByClassId(int32 ClassId);

protected:

	TMap<FString, TSubclassOf<ASandboxObject>> ObjectMapByClassName;

	virtual void SaveLevelJson();

	virtual void SaveLevelJsonExt(TSharedRef <TJsonWriter<TCHAR>> JsonWriter);

	virtual void LoadLevelJson();

	virtual void LoadLevelJsonExt(TSharedPtr<FJsonObject> JsonParsed);

};