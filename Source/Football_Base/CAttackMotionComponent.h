// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CAttackMotionComponent.generated.h"

// Attack animation state
enum class EAttackState
{
	None,
	Charge,       // 溜め
	Swing,        // 振り抜き
	OverShoot,    // 行き過ぎ
	Recover       // 揺れ戻し
};

// Component for managing attack motion animations
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FOOTBALL_BASE_API UCAttackMotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCAttackMotionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Start attack animation sequence
	void StartAttack();

private:
	// Apply motion offset to owner
	void ApplyMotion(float Alpha, const FVector& Offset);

private:
	// Current animation state
	EAttackState State = EAttackState::None;

	// Current state timer
	float Timer = 0.f;

	// Duration of charge phase
	UPROPERTY(EditAnywhere)
	float ChargeTime = 0.25f;

	// Duration of swing phase
	UPROPERTY(EditAnywhere)
	float SwingTime = 0.15f;

	// Duration of overshoot phase
	UPROPERTY(EditAnywhere)
	float OverShootTime = 0.1f;

	// Duration of recover phase
	UPROPERTY(EditAnywhere)
	float RecoverTime = 0.15f;

	// Initial position
	FVector StartLocation;
	
	// Position offsets - front motion (X-axis)
	UPROPERTY(EditAnywhere)
	FVector ChargeOffset = FVector(-20, 0, 0);     // 溜め (pull back)
	
	UPROPERTY(EditAnywhere)
	FVector SwingOffset = FVector(100, 0, 0);      // 振り抜き (swing forward)
	
	UPROPERTY(EditAnywhere)
	FVector OverOffset = FVector(30, 0, 0);        // オーバーシュート (overshoot forward)
	
	UPROPERTY(EditAnywhere)
	FVector RecoverOffset = FVector(-10, 0, 0);    // 揺れ戻し (recover back)

		
};
