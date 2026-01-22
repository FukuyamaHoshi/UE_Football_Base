// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "C_Manager_Anim_Instance.generated.h"

/**
 * 
 */
UCLASS()
class FOOTBALL_BASE_API UC_Manager_Anim_Instance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	int command = 0;

};
