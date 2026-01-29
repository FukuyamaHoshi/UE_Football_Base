// Fill out your copyright notice in the Description page of Project Settings.


#include "CDefenceMotionComponent.h"

// Sets default values for this component's properties
UCDefenceMotionComponent::UCDefenceMotionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCDefenceMotionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCDefenceMotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Skip if not defending
	if (State == EDefenceState::None) return;

	// Update timer
	Timer += DeltaTime;

	AActor* Owner = GetOwner();

	// Process current defence state
	switch (State)
	{
	case EDefenceState::Brace:
	{
		// Calculate progress (0 to 1)
		float Alpha = FMath::Clamp(Timer / BraceTime, 0.f, 1.f);
		// Apply easing curve for smooth motion
		float Eased = FMath::InterpEaseInOut(0.f, 1.f, Alpha, 2.0f);
		ApplyMotion(Eased, BraceOffset);

		// Transition to next state when complete
		if (Alpha >= 1.f)
		{
			State = EDefenceState::Impact;
			Timer = 0.f;
		}
		break;
	}

	case EDefenceState::Impact:
	{
		// Calculate progress
		float Alpha = FMath::Clamp(Timer / ImpactTime, 0.f, 1.f);
		// Apply sharp ease in for impact
		float Eased = FMath::InterpEaseIn(0.f, 1.f, Alpha, 3.0f);
		ApplyMotion(Eased, ImpactOffset);

		// Transition to absorb
		if (Alpha >= 1.f)
		{
			State = EDefenceState::Absorb;
			Timer = 0.f;
		}
		break;
	}

	case EDefenceState::Absorb:
	{
		// Calculate progress
		float Alpha = FMath::Clamp(Timer / AbsorbTime, 0.f, 1.f);
		// Apply ease out curve
		float Eased = FMath::InterpEaseOut(0.f, 1.f, Alpha, 2.0f);
		ApplyMotion(Eased, AbsorbOffset);

		// Transition to return
		if (Alpha >= 1.f)
		{
			State = EDefenceState::Return;
			Timer = 0.f;
		}
		break;
	}

	case EDefenceState::Return:
	{
		// Calculate progress
		float Alpha = FMath::Clamp(Timer / ReturnTime, 0.f, 1.f);
		// Apply ease out curve
		float Eased = FMath::InterpEaseOut(0.f, 1.f, Alpha, 1.5f);
		ApplyMotion(Eased, ReturnOffset);

		// End defence sequence
		if (Alpha >= 1.f)
		{
			State = EDefenceState::None;
		}
		break;
	}
	}

}

void UCDefenceMotionComponent::StartDefence()
{
	// Don't start if already defending
	if (State != EDefenceState::None) return;

	// Store starting position
	AActor* Owner = GetOwner();
	StartLocation = Owner->GetActorLocation();

	// Begin brace phase
	State = EDefenceState::Brace;
	Timer = 0.f;

}

void UCDefenceMotionComponent::ApplyMotion(float Alpha, const FVector& Offset)
{
	AActor* Owner = GetOwner();

	// Calculate new position based on offset and progress
	FVector NewLoc = StartLocation + Offset * Alpha;

	// Update actor position
	Owner->SetActorLocation(NewLoc);

}

