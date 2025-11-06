// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Tile.generated.h"

UCLASS()
class FOOTBALL_BASE_API AC_Tile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_Tile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UMaterial* pieceMoveMaterial = nullptr; // コマ移動マテリアル
	UMaterial* passRangeMaterial = nullptr; // パスレンジマテリアル
	UMaterial* markRangeMaterial = nullptr; // マークレンジマテリアル
	UMaterial* FWplayerPlaceMaterial = nullptr; // FWプレイヤー配置レンジマテリアル
	UMaterial* MFplayerPlaceMaterial = nullptr; // MFプレイヤー配置レンジマテリアル
	UMaterial* DFplayerPlaceMaterial = nullptr; // DFプレイヤー配置レンジマテリアル
	UMaterial* halfLernMaterial = nullptr; // ハーフレーンマテリアル
	// メッシュ
	UStaticMeshComponent* mainMesh = nullptr; // main(コマ移動、パスレンジ。マークレンジ)
	UStaticMeshComponent* subMesh = nullptr; // sub(プレイヤー配置レンジ)
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "myVariables")
	int tileNo = 0; // タイルNo

	// コマ移動マテリアルをセット
	void SetMaterial();
	// メインのオーバーレイマテリアルを削除する
	void RemoveMainMaterial();
	// サブのオーバーレイマテリアルを削除する
	void RemoveSubMaterial();
	// パスレンジマテリアルをセット
	void SetPassRangeMaterial();
	// マークレンジマテリアルをセット
	void SetMarkRangeMaterial();
	// プレイヤー配置レンジマテリアルをセット
	// - FW -
	void SetFWPlayerPlaceRangeMaterial();
	// - MF -
	void SetMFPlayerPlaceRangeMaterial();
	// - DF -
	void SetDFPlayerPlaceRangeMaterial();
	// ハーフレーンマテリアルをセット
	void SetHalfLernMaterial();
};
