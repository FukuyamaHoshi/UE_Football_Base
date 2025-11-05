// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "C_Player_Anim_Instance.generated.h"

/**
 * 
 */
UCLASS()
class FOOTBALL_BASE_API UC_Player_Anim_Instance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool isKeep = false;
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool isRun = false;
	
};
