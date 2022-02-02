
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HeroAnimInstance.generated.h"


UCLASS()
class IBHERO_API UHeroAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly)
		float Speed;
	UPROPERTY(BlueprintReadOnly)
		bool bFly;
	UPROPERTY(BlueprintReadOnly)
		bool bFalling;


};
