// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CDefenceMotionComponent.generated.h"

// Defence animation state
enum class EDefenceState
{
	None,
	Brace,        // ç\Ç¶
	Impact,       // è’åÇéÛÇØ
	Absorb,       // ãzé˚
	Return        // ñﬂÇË
};

// Component for managing defence motion animations
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FOOTBALL_BASE_API UCDefenceMotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCDefenceMotionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Start defence animation sequence
	void StartDefence();

private:
	// Apply motion offset to owner
	void ApplyMotion(float Alpha, const FVector& Offset);

private:
	// Current animation state
	EDefenceState State = EDefenceState::None;

	// Current state timer
	float Timer = 0.f;

	// Duration of brace phase
	UPROPERTY(EditAnywhere)
	float BraceTime = 0.15f;

	// Duration of impact phase
	UPROPERTY(EditAnywhere)
	float ImpactTime = 0.1f;

	// Duration of absorb phase
	UPROPERTY(EditAnywhere)
	float AbsorbTime = 0.15f;

	// Duration of return phase
	UPROPERTY(EditAnywhere)
	float ReturnTime = 0.2f;

	// Initial position
	FVector StartLocation;
	
	// Position offsets - defence motion (backward)
	UPROPERTY(EditAnywhere)
	FVector BraceOffset = FVector(-10, 0, 0);      // ç\Ç¶ (brace position)
	
	UPROPERTY(EditAnywhere)
	FVector ImpactOffset = FVector(-50, 0, 0);     // è’åÇéÛÇØ (pushed back by impact)
	
	UPROPERTY(EditAnywhere)
	FVector AbsorbOffset = FVector(-20, 0, 0);     // ãzé˚ (absorbing force)
	
	UPROPERTY(EditAnywhere)
	FVector ReturnOffset = FVector(0, 0, 0);       // ñﬂÇË (return to start)
		
};
