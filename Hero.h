#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Hero.generated.h"

UCLASS()
class IBHERO_API AHero : public ACharacter
{
	GENERATED_BODY()

private:
	float MaxWalkSpeed;
	float MaxFlySpeed;
public:
	AHero();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* FollowCamera;

protected:
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void Fly();
	void Jump();
	void StartFlying();
	void StopFlying();
	void StartSprint();
	void StopSprint();
	void EnableMovement();
	void StartFalling();
	void StopFalling();
		UFUNCTION()
		void OnTriggerEnter(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;
	UPROPERTY(EditAnywhere)
		bool bMovement;
	UPROPERTY(EditAnywhere)
		bool bSprint;
	UPROPERTY(EditAnywhere)
		bool bFly;
	UPROPERTY(EditAnywhere)
		bool bFalling;
	UPROPERTY(EditAnywhere)
		class UAnimMontage* JumpAnimation;
	UPROPERTY(EditAnywhere)
		class UAnimMontage* HardLanding;
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FTimerHandle TimerHandle;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* Trigger;

	UPROPERTY(EditAnywhere)
		class UParticleSystem* SmokeParticleEffect;
	UPROPERTY(EditAnywhere)
		class UParticleSystem* LandingEffect;

	
};
