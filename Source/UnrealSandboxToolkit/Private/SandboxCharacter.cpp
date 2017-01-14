// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealSandboxToolkitPrivatePCH.h"
#include "SandboxCharacter.h"
#include "SandboxPlayerController.h"


ASandboxCharacter::ASandboxCharacter() {

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->RelativeLocation = FVector(0, 0, 0); // Position the camera

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("head"));
	FirstPersonCamera->RelativeLocation = FVector(10.0f, 32.0f, 0.f); // Position the camera
	FirstPersonCamera->RelativeRotation = FRotator(0, 90, -90);
	FirstPersonCamera->bUsePawnControlRotation = true;

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASandboxCharacter::OnHit);

	// initial view
	CurrentPlayerView = PlayerView::TOP_DOWN;
	InitTopDownView();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	MaxZoom = 500;
	MinZoom = 100;
	ZoomStep = 50;

	WalkSpeed = 200;
	RunSpeed = 600;

	InteractionTargetLength = 200;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ASandboxCharacter::BeginPlay() {
	Super::BeginPlay();
	
	CurrentPlayerView = InitialView;

	if (InitialView == PlayerView::TOP_DOWN) {
		InitTopDownView();
	}

	if (InitialView == PlayerView::THIRD_PERSON) {
		InitThirdPersonView();
	}

	if (InitialView == PlayerView::FIRST_PERSON) {
		InitFirstPersonView();
	}
}

void ASandboxCharacter::Tick( float DeltaTime ) {
	Super::Tick( DeltaTime );

	if (IsDead()) {
		FVector MeshLoc = GetMesh()->GetSocketLocation(TEXT("pelvis"));
		GetCapsuleComponent()->SetWorldLocation(MeshLoc - InitialMeshTransform.GetLocation());
	}
}

void ASandboxCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent) {
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("ZoomIn", IE_Released, this, &ASandboxCharacter::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Released, this, &ASandboxCharacter::ZoomOut);

	InputComponent->BindAction("Boost", IE_Pressed, this, &ASandboxCharacter::BoostOn);
	InputComponent->BindAction("Boost", IE_Released, this, &ASandboxCharacter::BoostOff);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ASandboxCharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ASandboxCharacter::StopJumping);

	InputComponent->BindAxis("MoveForward", this, &ASandboxCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ASandboxCharacter::MoveRight);

	InputComponent->BindAction("Test", IE_Pressed, this, &ASandboxCharacter::Test);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &ASandboxCharacter::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ASandboxCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &ASandboxCharacter::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ASandboxCharacter::LookUpAtRate);
}


void ASandboxCharacter::BoostOn() {
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

}

void ASandboxCharacter::BoostOff() {
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ASandboxCharacter::Jump() {
	if (IsDead()) return;

	Super::Jump();
}

void ASandboxCharacter::StopJumping() {
	if (IsDead()) return;

	Super::StopJumping();
}


void ASandboxCharacter::ZoomIn() {
	if (GetCameraBoom() == NULL) return;
	if (CurrentPlayerView == PlayerView::FIRST_PERSON) return;

	if (GetCameraBoom()->TargetArmLength > MinZoom) {
		GetCameraBoom()->TargetArmLength -= ZoomStep;
	} else {
		InitFirstPersonView();
	}

	//UE_LOG(LogTemp, Warning, TEXT("ZoomIn: %f"), GetCameraBoom()->TargetArmLength);
}

void ASandboxCharacter::ZoomOut() {
	if (GetCameraBoom() == NULL) return;

	if (CurrentPlayerView == PlayerView::FIRST_PERSON) {
		InitThirdPersonView();
		return;
	};

	if (GetCameraBoom()->TargetArmLength < MaxZoom) {
		GetCameraBoom()->TargetArmLength += ZoomStep;
	}

	//UE_LOG(LogTemp, Warning, TEXT("ZoomOut: %f"), GetCameraBoom()->TargetArmLength);
}

void ASandboxCharacter::InitTopDownView() {

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does

	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller
	CameraBoom->ProbeSize = 0;
	CameraBoom->RelativeLocation = FVector(0, 0, 0);

	FirstPersonCamera->Deactivate();
	FollowCamera->Activate();

	bUseControllerRotationYaw = false;

	CurrentPlayerView = PlayerView::TOP_DOWN;

	ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController());
	if (Controller != NULL) {
		Controller->ShowMouseCursor(true);
		Controller->OpenCrosshairWidget();
	}
}

void ASandboxCharacter::InitThirdPersonView() {

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->ProbeSize = 12;
	CameraBoom->RelativeLocation = FVector(0, 0, 64);

	FirstPersonCamera->Deactivate();
	FollowCamera->Activate();

	bUseControllerRotationYaw = false;

	CurrentPlayerView = PlayerView::THIRD_PERSON;

	ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController());
	if (Controller != NULL) {
		Controller->ShowMouseCursor(false);

		if (CrosshairWidget) {
			Controller->CloseCrosshairWidget();
		}
	}
}

void ASandboxCharacter::InitFirstPersonView() {

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	FirstPersonCamera->Activate();
	FollowCamera->Deactivate();

	bUseControllerRotationYaw = true;

	CurrentPlayerView = PlayerView::FIRST_PERSON;

	ASandboxPlayerController* Controller = Cast<ASandboxPlayerController>(GetController());
	if (Controller != NULL) {
		Controller->ShowMouseCursor(false);

		if (CrosshairWidget) {
			Controller->OpenCrosshairWidget();
		}
	}
}

void ASandboxCharacter::AddControllerYawInput(float Val) {
	AController* controller = (AController*)GetController();
	if (controller == NULL) { return; }

	ASandboxPlayerController* SandboxController = Cast<ASandboxPlayerController>(GetController());
	if (SandboxController != NULL && SandboxController->IsGameInputBlocked()) return;

	if (CurrentPlayerView == PlayerView::TOP_DOWN) return;

	Super::AddControllerYawInput(Val);

}

void ASandboxCharacter::AddControllerPitchInput(float Val) {
	AController* controller = (AController*)GetController();
	if (controller == NULL) { return; }

	ASandboxPlayerController* SandboxController = Cast<ASandboxPlayerController>(GetController());
	if (SandboxController != NULL && SandboxController->IsGameInputBlocked()) return;

	if (CurrentPlayerView == PlayerView::TOP_DOWN) return;

	Super::AddControllerPitchInput(Val);
}

void ASandboxCharacter::TurnAtRate(float Rate) {
	AController* controller = (AController*)GetController();
	if (controller == NULL) { return; }

	ASandboxPlayerController* SandboxController = Cast<ASandboxPlayerController>(GetController());
	if (SandboxController != NULL && SandboxController->IsGameInputBlocked()) return;

	if (CurrentPlayerView == PlayerView::TOP_DOWN) return;

	// calculate delta for this frame from the rate information
	//AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASandboxCharacter::LookUpAtRate(float Rate) {
	AController* controller = (AController*)GetController();
	if (controller == NULL) { return; }

	ASandboxPlayerController* SandboxController = Cast<ASandboxPlayerController>(GetController());
	if (SandboxController != NULL && SandboxController->IsGameInputBlocked()) return;

	if (CurrentPlayerView == PlayerView::TOP_DOWN) return;

	// calculate delta for this frame from the rate information
	//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void ASandboxCharacter::MoveForward(float Value) {
	AController* controller = (AController*)GetController();
	if (controller == NULL) { return; }

	ASandboxPlayerController* SandboxController = Cast<ASandboxPlayerController>(GetController());
	if (SandboxController != NULL && SandboxController->IsGameInputBlocked()) return;

	if (IsDead()) { return; };

	if (CurrentPlayerView == PlayerView::THIRD_PERSON) {
		if (Value != 0.0f)	{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
	}

	if (CurrentPlayerView == PlayerView::FIRST_PERSON) {
		if (Value != 0.0f)	{
			// add movement in that direction
			AddMovementInput(GetActorForwardVector(), Value);
		}
	}
}

void ASandboxCharacter::MoveRight(float Value) {
	AController* controller = (AController*)GetController();
	if (controller == NULL) { return; }

	ASandboxPlayerController* SandboxController = Cast<ASandboxPlayerController>(GetController());
	if (SandboxController != NULL && SandboxController->IsGameInputBlocked()) return;

	if (IsDead()) { return; };

	if (CurrentPlayerView == PlayerView::THIRD_PERSON) {
		if (Value != 0.0f) {
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(Direction, Value);
		}
	}

	if (CurrentPlayerView == PlayerView::FIRST_PERSON) {
		if (Value != 0.0f) {
			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value);
		}
	}
}

PlayerView ASandboxCharacter::GetSandboxPlayerView() {
	return CurrentPlayerView;
}

void ASandboxCharacter::SetSandboxPlayerView(PlayerView SandboxView) {
	CurrentPlayerView = SandboxView;
}

void ASandboxCharacter::Test() {
	if(!IsDead()) {
		Kill();
	} else {
		LiveUp();
	}
}

void ASandboxCharacter::Kill() {
	if (!IsDead()) {
		InitialMeshTransform = GetMesh()->GetRelativeTransform();
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("pelvis"), 1);
		bIsDead = true;
	}
}

void ASandboxCharacter::LiveUp() {
	if (IsDead()) {
		GetMesh()->SetSimulatePhysics(false);
		bIsDead = false;
		GetMesh()->AttachTo(GetCapsuleComponent(), NAME_None, EAttachLocation::SnapToTargetIncludingScale);
		GetMesh()->SetRelativeTransform(InitialMeshTransform);
	}
}

void ASandboxCharacter::OnHit(class UPrimitiveComponent* HitComp, class AActor* Actor, class UPrimitiveComponent* Other, FVector Impulse, const FHitResult & HitResult) {
	//UE_LOG(LogTemp, Warning, TEXT("hit velocity -> %f %f %f"), GetCapsuleComponent()->GetComponentVelocity().X, GetCapsuleComponent()->GetComponentVelocity().Y, GetCapsuleComponent()->GetComponentVelocity().Z);
}