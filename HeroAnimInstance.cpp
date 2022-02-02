#include "Hero.h"

#include "HeroAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"

void UHeroAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (TryGetPawnOwner()) {
		Speed = TryGetPawnOwner()->GetVelocity().Size();
		bFly = TryGetPawnOwner()->GetMovementComponent()->IsFlying();
		bFalling = TryGetPawnOwner()->GetMovementComponent()->IsFalling();
		
	

	}
}
