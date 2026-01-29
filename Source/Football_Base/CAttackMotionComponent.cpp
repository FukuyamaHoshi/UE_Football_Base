// Fill out your copyright notice in the Description page of Project Settings.


#include "CAttackMotionComponent.h"

// Sets default values for this component's properties
UCAttackMotionComponent::UCAttackMotionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCAttackMotionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCAttackMotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Skip if not attacking
	if (State == EAttackState::None) return;

	// Update timer
	Timer += DeltaTime;

	AActor* Owner = GetOwner();

	// Process current attack state
	switch (State)
	{
	case EAttackState::Charge:
	{
		// Calculate progress (0 to 1)
		float Alpha = FMath::Clamp(Timer / ChargeTime, 0.f, 1.f);
		// Apply easing curve for smooth motion
		float Eased = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.5f);
		ApplyMotion(Eased, ChargeOffset);

		// Transition to next state when complete
		if (Alpha >= 1.f)
		{
			State = EAttackState::Swing;
			Timer = 0.f;
		}
		break;
	}

	case EAttackState::Swing:
	{
		// Calculate progress
		float Alpha = FMath::Clamp(Timer / SwingTime, 0.f, 1.f);
		// Apply easing curve
		float Eased = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 1.8f);
		ApplyMotion(Eased, SwingOffset);

		// Transition to overshoot
		if (Alpha >= 1.f)
		{
			State = EAttackState::OverShoot;
			Timer = 0.f;
		}
		break;
	}

	case EAttackState::OverShoot:
	{
		// Calculate progress
		float Alpha = FMath::Clamp(Timer / OverShootTime, 0.f, 1.f);
		// Apply ease out curve
		float Eased = FMath::InterpEaseOut(0.f, 1.f, Alpha, 2.0f);
		ApplyMotion(Eased, OverOffset);

		// Transition to recovery
		if (Alpha >= 1.f)
		{
			State = EAttackState::Recover;
			Timer = 0.f;
		}
		break;
	}

	case EAttackState::Recover:
	{
		// Calculate progress
		float Alpha = FMath::Clamp(Timer / RecoverTime, 0.f, 1.f);
		// Apply ease out curve
		float Eased = FMath::InterpEaseOut(0.f, 1.f, Alpha, 2.0f);
		ApplyMotion(Eased, RecoverOffset);

		// End attack sequence
		if (Alpha >= 1.f)
		{
			State = EAttackState::None;
		}
		break;
	}
	}

}

void UCAttackMotionComponent::StartAttack()
{
	// Don't start if already attacking
	if (State != EAttackState::None) return;

	// Store starting position
	AActor* Owner = GetOwner();
	StartLocation = Owner->GetActorLocation();

	// Begin charge phase
	State = EAttackState::Charge;
	Timer = 0.f;

}

void UCAttackMotionComponent::ApplyMotion(float Alpha, const FVector& Offset)
{
	AActor* Owner = GetOwner();

	// Calculate new position based on offset and progress
	FVector NewLoc = StartLocation + Offset * Alpha;

	// Update actor position
	Owner->SetActorLocation(NewLoc);

}

