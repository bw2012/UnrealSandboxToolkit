

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "SandboxLevelController.h"
#include "SandboxObject.h"

ASandboxLevelController::ASandboxLevelController() {
	MapName = TEXT("World 0");
}

void ASandboxLevelController::BeginPlay() {
	Super::BeginPlay();
	LoadLevelJson();
}

void ASandboxLevelController::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	SaveLevelJson();
}

void ASandboxLevelController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ASandboxLevelController::SaveLevelJson() {
	UE_LOG(LogTemp, Warning, TEXT("----------- save level json -----------"));

	FString JsonStr;
	FString FileName = TEXT("level.json");
	FString SavePath = FPaths::GameSavedDir();
	FString FullPath = SavePath + TEXT("/Map/") + MapName + TEXT("/") + FileName;

	//UE_LOG(LogTemp, Warning, TEXT("level json path -> %s"), *FullPath);

	TSharedRef <TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&JsonStr);
	JsonWriter->WriteObjectStart();
	JsonWriter->WriteArrayStart("SandboxObjectList");

	for (TActorIterator<ASandboxObject> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		ASandboxObject* SandboxObject = Cast<ASandboxObject>(*ActorItr);
		if (SandboxObject) {
			FString SandboxObjectName = SandboxObject->GetName();
			FString ClassName = SandboxObject->GetClass()->GetName();

			FVector Location = SandboxObject->GetActorLocation();
			FRotator Rotation = SandboxObject->GetActorRotation();
			FVector Scale = SandboxObject->GetActorScale();

			JsonWriter->WriteObjectStart();
			JsonWriter->WriteObjectStart("Object");

			JsonWriter->WriteValue("Name", SandboxObjectName);
			JsonWriter->WriteValue("Class", ClassName);
			JsonWriter->WriteValue("ClassId", SandboxObject->GetSandboxClassId());
			JsonWriter->WriteValue("TypeId", SandboxObject->GetSandboxClassId());

			JsonWriter->WriteArrayStart("Location");
			JsonWriter->WriteValue(Location.X);
			JsonWriter->WriteValue(Location.Y);
			JsonWriter->WriteValue(Location.Z);
			JsonWriter->WriteArrayEnd();

			JsonWriter->WriteArrayStart("Rotation");
			JsonWriter->WriteValue(Rotation.Pitch);
			JsonWriter->WriteValue(Rotation.Yaw);
			JsonWriter->WriteValue(Rotation.Roll);
			JsonWriter->WriteArrayEnd();

			JsonWriter->WriteArrayStart("Scale");
			JsonWriter->WriteValue(Scale.X);
			JsonWriter->WriteValue(Scale.Y);
			JsonWriter->WriteValue(Scale.Z);
			JsonWriter->WriteArrayEnd();

			JsonWriter->WriteObjectEnd();
			JsonWriter->WriteObjectEnd();
		}
	}

	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	FFileHelper::SaveStringToFile(*JsonStr, *FullPath);
}

void ASandboxLevelController::LoadLevelJson() {
	if (!ObjectMap) {
		UE_LOG(LogTemp, Error, TEXT("ASandboxLevelController::LoadLevelJson() -----  ObjectMap == NULL"));
		return;
	}

	ObjectMapByClassName.Empty();
	for (auto& Elem : ObjectMap->ObjectMap) {
		TSubclassOf<ASandboxObject> SandboxObject = Elem.Value;
		if (SandboxObject) {
			FString ClassName = SandboxObject->ClassDefaultObject->GetClass()->GetName();
			UE_LOG(LogTemp, Warning, TEXT("ClassName -> %s"), *ClassName);
			ObjectMapByClassName.Add(ClassName, SandboxObject);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("----------- load level json -----------"));

	FString FileName = TEXT("level.json");
	FString SavePath = FPaths::GameSavedDir();
	FString FullPath = SavePath + TEXT("/Map/") + MapName + TEXT("/") + FileName;

	FString JsonRaw;
	if (!FFileHelper::LoadFileToString(JsonRaw, *FullPath)) {
		UE_LOG(LogTemp, Error, TEXT("Error loading json file"));
	}

	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonRaw);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed)) {

		TArray <TSharedPtr<FJsonValue>> SandboxObjectList = JsonParsed->GetArrayField("SandboxObjectList");
		for (int Idx = 0; Idx < SandboxObjectList.Num(); Idx++) {
			TSharedPtr<FJsonObject> ObjPtr = SandboxObjectList[Idx]->AsObject();
			TSharedPtr<FJsonObject> SandboxObjectPtr = ObjPtr->GetObjectField(TEXT("Object"));
			FString ClassName = SandboxObjectPtr->GetStringField(TEXT("Class"));

			FVector Location;
			TArray <TSharedPtr<FJsonValue>> LocationValArray = SandboxObjectPtr->GetArrayField("Location");
			Location.X = LocationValArray[0]->AsNumber();
			Location.Y = LocationValArray[1]->AsNumber();
			Location.Z = LocationValArray[2]->AsNumber();

			FRotator Rotation;
			TArray <TSharedPtr<FJsonValue>>  RotationValArray = SandboxObjectPtr->GetArrayField("Rotation");
			Rotation.Pitch = RotationValArray[0]->AsNumber();
			Rotation.Yaw = RotationValArray[1]->AsNumber();
			Rotation.Roll = RotationValArray[2]->AsNumber();

			FVector Scale;
			TArray <TSharedPtr<FJsonValue>> ScaleValArray = SandboxObjectPtr->GetArrayField("Scale");
			Scale.X = ScaleValArray[0]->AsNumber();
			Scale.Y = ScaleValArray[1]->AsNumber();
			Scale.Z = ScaleValArray[2]->AsNumber();

			//UE_LOG(LogTemp, Warning, TEXT("Location: %f %f %f"), Location.X, Location.Y, Location.Z);

			// get object actor
			if (ObjectMapByClassName.Contains(ClassName)) {
				TSubclassOf<ASandboxObject> SandboxObject = ObjectMapByClassName[ClassName];
				UClass* SpawnClass = SandboxObject->ClassDefaultObject->GetClass();
				FTransform Transform(Rotation, Location, Scale);
				GetWorld()->SpawnActor(SpawnClass, &Transform);
			}
		}
	}
}