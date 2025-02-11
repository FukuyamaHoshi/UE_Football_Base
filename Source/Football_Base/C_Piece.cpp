// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Piece.h"
#include <Kismet/KismetMathLibrary.h>
#include "C_Common.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraDataInterfaceVectorField.h>

// Sets default values
AC_Piece::AC_Piece()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AC_Piece::BeginPlay()
{
	Super::BeginPlay();
	
    // ** プレイヤー間のライン(niagara component)を取得 **
    TArray<UNiagaraComponent*> Components; // component配列
    GetComponents<UNiagaraComponent>(Components); // componentを取得

    mainLine = Components[0]; // セット
    subLine = Components[1]; // セット
    // **
}

// Called every frame
void AC_Piece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (isMoving) MoveTo();

}

// Called to bind functionality to input
void AC_Piece::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// 指定された位置へ動く
void AC_Piece::MoveTo()
{
    // 引数セット
    FVector currentLocation = GetActorLocation(); // 現在の位置を取得
    float tile = GetWorld()->GetDeltaSeconds(); // 前のフレームからの経過時間を取得

    // ** 移動終了処理 **
    if (FVector::Distance(currentLocation, targetLocation) < 3.0) {
        SetActorLocation(targetLocation); // 位置をターゲット位置へ
        targetLocation = FVector(0, 0, 0); // ターゲット位置リセット
        isMoving = false; // 移動終了

        return;
    }

    // ターゲットまでの位置取得
    FVector newLocation = UKismetMathLibrary::VInterpTo(currentLocation, targetLocation, tile, C_Common::PIECE_SPEED);

    // 自身に新しい位置を適用
    SetActorLocation(newLocation);
}

void AC_Piece::SetMoveTo(FVector tLocation)
{
    tLocation.Z = C_Common::BASE_LOCATION_Z; // ** Zの位置を修正 **
    targetLocation = tLocation; // 目標位置セット
    isMoving = true; // 移動開始
}

// 線を表示する
void AC_Piece::SetDrawLineTo(FVector mainTargetLocation, FLinearColor lineColor, FVector subTargetLocation)
{
    // main
    if (mainLine != nullptr) {
        mainLine->SetVariablePosition(FName("End_Pos"), mainTargetLocation);
        mainLine->SetVariableLinearColor(FName("Color"), lineColor);
        mainLine->SetVisibility(true);
    }
    // sub
    if (subLine != nullptr && subTargetLocation != FVector(0, 0, 0)) {
        subLine->SetVariablePosition(FName("End_Pos"), subTargetLocation);
        subLine->SetVariableLinearColor(FName("Color"), lineColor);
        subLine->SetVisibility(true);
    }
}

