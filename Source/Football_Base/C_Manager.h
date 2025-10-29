// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "C_Manager.generated.h"

UCLASS()
class FOOTBALL_BASE_API AC_Manager : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AC_Manager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// アウトライン表示
	void VisibleOutline();
	// アウトライン非表示
	void HiddenOutline();

private:
	USkeletalMeshComponent* mySkeltalMesh = nullptr; // メッシュ
	UMaterial* outLineMaterial = nullptr; // アウトラインマテリアル

};
