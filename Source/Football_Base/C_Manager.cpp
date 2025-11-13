// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Manager.h"
#include <Kismet/KismetSystemLibrary.h>
#include "C_Common.h"

// Sets default values
AC_Manager::AC_Manager()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_Manager::BeginPlay()
{
	Super::BeginPlay();

	// メッシュ取得
	mySkeltalMesh = FindComponentByClass<USkeletalMeshComponent>();

	// *** アウトラインマテリアル取得 ***
	// (コンストラクタ以外のマテリアルはLoadObject使用)
	outLineMaterial = LoadObject<UMaterial>(NULL, TEXT("/Game/Materials/M_Outline.M_Outline"), NULL, LOAD_None, NULL);
	// ***

	// *** アニメーションインスタンス取得 ***
	if (mySkeltalMesh)
	{
		// 2. アニメーションインスタンスを取得
		UAnimInstance* _animInstance = mySkeltalMesh->GetAnimInstance();

		// 3. アニメーションインスタンスがUMyAnimInstance型であることを確認
		if (_animInstance)
		{
			animInstance = Cast<UC_Manager_Anim_Instance>(_animInstance);
		}
	}
	// ***

	// *** アニメーションモンタージュ取得 (トーマス) ***
	idleAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Thomas/AM_Thomas_Idle.AM_Thomas_Idle"), NULL, LOAD_None, NULL);
	laneAttackAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Thomas/AM_Thomas_Lane_Attack.AM_Thomas_Lane_Attack"), NULL, LOAD_None, NULL);
	longAttackAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Thomas/AM_Thomas_Long_Attack.AM_Thomas_Long_Attack"), NULL, LOAD_None, NULL);
	escapePressAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Thomas/AM_Thomas_Escape_Press.AM_Thomas_Escape_Press"), NULL, LOAD_None, NULL);
	cheer1Anim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Thomas/AM_Cheer_1.AM_Cheer_1"), NULL, LOAD_None, NULL);
	cheer2Anim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Thomas/AM_Cheer_2.AM_Cheer_2"), NULL, LOAD_None, NULL);
	// ***
}

// Called every frame
void AC_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AC_Manager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// アウトライン表示
void AC_Manager::VisibleOutline()
{
	// マテリアルをセット
	if (mySkeltalMesh && outLineMaterial) mySkeltalMesh->SetOverlayMaterial(outLineMaterial);
}

// アウトライン非表示
void AC_Manager::HiddenOutline()
{
	// マテリアルを削除
	if (mySkeltalMesh) mySkeltalMesh->SetOverlayMaterial(nullptr);
}

// コマンドアニメーション切り替え
void AC_Manager::ChangeAnim(int command)
{
	if (animInstance == nullptr) return;

	// ABP切り替え
	animInstance->command = command;


	// モンタージュ切り替え
	if (command == C_Common::ESCAPE_PRESSING_COMMAND_NO) { // プレス回避
		if (escapePressAnim) animInstance->Montage_Play(escapePressAnim);
	}
	else if (command == C_Common::LONG_ATTACK_COMMAND_NO) { // ロングアタック
		if (longAttackAnim) animInstance->Montage_Play(longAttackAnim);
	}
	else if (command == C_Common::LANE_ATTACK_COMMAND_NO) { // レーンアタック
		if (laneAttackAnim) animInstance->Montage_Play(laneAttackAnim);
	}
	else { // アイドル
		if (idleAnim) animInstance->Montage_Play(idleAnim);
	}
}

// 喜びアニメーション
void AC_Manager::CheerAnim()
{
	bool _b = FMath::RandBool(); // ランダム

	if (_b) {
		if (cheer1Anim && animInstance) animInstance->Montage_Play(cheer1Anim);
	}
	else {
		if (cheer2Anim && animInstance) animInstance->Montage_Play(cheer2Anim);
	}
}
