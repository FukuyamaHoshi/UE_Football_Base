// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_Tile.h"
#include "C_Ball.h"
#include "GameFramework/PlayerController.h"
#include "C_My_Player_Controller.generated.h"

/**
 * 
 */
UCLASS()
class FOOTBALL_BASE_API AC_My_Player_Controller : public APlayerController
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// 入力設定
	void SetupInput();
	// 左クリック(プレス)イベント
	void PressedLeft();
	// 左クリック(リリース)イベント
	void ReleasedLeft();
	// 自分のチームのコマを選択
	AActor* SelectHomePiece();
	// 位置からタイルＮｏ取得
	int GetTileNoFromLocation(float x, float y);
	// マウス位置のオブジェクトを取得
	// ( return: bool(取得できたか), FHitResult&(取得したオブジェクト情報)<参照渡し> )
	bool GetObjectFromMouseLocation(TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes, FHitResult& outHit);

	
	AActor* selectedPlayer; // 選択されたプレイヤー
	bool isGrap = false; // Grap(プレイヤー選択中)フラグ
	int currentHoverTileNo = 0; // 現在ホバーしているタイルNo
	int selectedPlayerTileNo = 0; // 選択しているプレイヤーのタイルNo
	int overlayTileNo = 0; // 現在光っているタイルNo
	
	TArray <AC_Tile*> allTiles; // すべてのタイル配列
	AC_Tile* overlayTile; // 光っているタイル

	UMaterial* playerSelectedDecal = nullptr; // プレイヤー選択デカール
	UDecalComponent* currentDecal = nullptr; // 現在表示しているデカール

	AC_Ball* ball; // ボール
};
