// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Manager.h"
#include <Kismet/KismetSystemLibrary.h>

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

