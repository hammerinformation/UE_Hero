
#include "Hero.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AHero::AHero()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 440.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	GetCharacterMovement()->MaxWalkSpeed = 1500.0f;
	GetCharacterMovement()->MaxFlySpeed = 3500.0f;
	GetCharacterMovement()->MaxAcceleration = 500.0f;
	GetCharacterMovement()->BrakingDecelerationFlying = 1500.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1500.0f;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(RootComponent);
}

void AHero::BeginPlay()
{
	Super::BeginPlay();

	this->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	this->MaxFlySpeed = GetCharacterMovement()->MaxFlySpeed;
	bFly = false;
	bMovement = true;
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AHero::OnTriggerEnter);

}

void AHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (GetActorLocation().Z < -1000.0f) {
		SetActorLocation(FVector(620, 620, 120));

	}


}

void AHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AHero::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Fly", IE_Pressed, this, &AHero::Fly);
	PlayerInputComponent->BindAction("StartSprint", IE_Pressed, this, &AHero::StartSprint);
	PlayerInputComponent->BindAction("StopSprint", IE_Released, this, &AHero::StopSprint);
	PlayerInputComponent->BindAxis("MoveForward", this, &AHero::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHero::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AHero::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AHero::LookUpAtRate);

}

void AHero::TurnAtRate(float Rate)
{

	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AHero::LookUpAtRate(float Rate)
{

	if (bFly && Rate != 0.0f && GetCharacterMovement()->Velocity.Size() > 1600.0f) {
		AddMovementInput(GetActorUpVector(), Rate * -5);

	}
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AHero::Fly()
{
	bFly = !bFly;
	if (bFly) { StartFlying(); }
	else { StopFlying(); }
}

void AHero::Jump()
{
	//Play Jump Animation
	if (bFly) { return; }
	if (bMovement == false) {
		Fly();
		return;
	}
	if (JumpAnimation) {
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance != nullptr) {
			AnimInstance->Montage_Play(JumpAnimation, 1.0f);
		}
		bMovement = false;
	}
	ACharacter::Jump();
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AHero::EnableMovement, 111.0f, true, JumpAnimation->GetPlayLength());

}

void AHero::StartFlying()
{
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 140.0f, 0.0f);
	GetCharacterMovement()->MaxAcceleration = 2500.0f;

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

}

void AHero::StopFlying()
{
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->MaxAcceleration = 500.0f;


	if (GetActorLocation().Z < 250.0f) {
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	}
	else if (GetActorLocation().Z > 250.0f) {
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		StartFalling();

	}
}

void AHero::StartSprint()
{
	GetCharacterMovement()->MaxAcceleration = 2000.0f;
	GetCharacterMovement()->MaxWalkSpeed = this->MaxWalkSpeed * 1.5f;
	GetCharacterMovement()->MaxFlySpeed = this->MaxFlySpeed * 1.5f;
}

void AHero::StopSprint()
{
	GetCharacterMovement()->MaxAcceleration = 500.0f;
	GetCharacterMovement()->MaxWalkSpeed = this->MaxWalkSpeed;
	GetCharacterMovement()->MaxFlySpeed = this->MaxFlySpeed;

}

void AHero::EnableMovement()
{
	bMovement = true;
	GetWorldTimerManager().ClearTimer(TimerHandle);
}

void AHero::StartFalling()
{
	bMovement = false;
	bFalling = true;


}

void AHero::StopFalling()
{

	if (GetCharacterMovement()->IsFalling() && this->bFalling) {
		if (HardLanding) {
			bFalling = false;
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			if (AnimInstance != nullptr) {
				AnimInstance->Montage_Play(HardLanding, 1.0f);
			}
		}
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AHero::EnableMovement, 111.0f, true, HardLanding->GetPlayLength());

		if (LandingEffect) {

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LandingEffect, FVector(GetActorLocation().X, GetActorLocation().Y,32));

		}

	}


}

void AHero::OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetName() == FString(TEXT("Floor"))) {

		UE_LOG(LogTemp, Error, TEXT("Floor "));
		StopFalling();
		if (SmokeParticleEffect && GetCharacterMovement()->Velocity.Size() < 1000.0f) {

		//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeParticleEffect, FVector(GetActorLocation().X, GetActorLocation().Y, OtherActor->GetActorLocation().Z));

		}
	}
}



void AHero::MoveForward(float Value)
{
	if (bMovement == false) { return; }

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AHero::MoveRight(float Value)
{
	if (bMovement == false) { return; }

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}