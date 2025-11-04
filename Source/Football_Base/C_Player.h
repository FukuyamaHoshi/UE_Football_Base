// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "C_Soccer_Ball.h"
#include "C_Player_Anim_Instance.h"
#include "C_Player.generated.h"

UCLASS()
class FOOTBALL_BASE_API AC_Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AC_Player();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool isBallHolder = false; // ボールホルダー判定

	// メッシュを表示する
	void DisplayMesh();
	// ショートパス
	void ShotPass(AC_Player* targetPlayer);
	// ロングパス
	void LongPass(AC_Player* targetPlayer);
	// トラップ
	void Trap(AC_Player* fromPlayer);
	// ボール保持
	void BallKeeping();

private:
	USkeletalMeshComponent* myMesh = nullptr; // メッシュ
	AC_Soccer_Ball* ball = nullptr; // ボール
	UAnimMontage* shortPassAnim = nullptr; // アニメーション(ショートパス)
	UAnimMontage* trapAnim = nullptr; // アニメーション(トラップ)
	bool isTrap = false; // トラップアニメーションするか
	UC_Player_Anim_Instance* playerAnimInstance = nullptr; // アニメーションインスタンス
	bool isStanding = true; // スタンディング中か (ボールキープアニメーションを一度のみ実行するため)
};
