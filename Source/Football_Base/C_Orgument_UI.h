// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/TextBlock.h>
#include <Components/MultiLineEditableTextBox.h>
#include <Components/Button.h>
#include <Components/Image.h>
#include "C_Orgument_UI.generated.h"

/**
 * 
 */
UCLASS()
class FOOTBALL_BASE_API UC_Orgument_UI : public UUserWidget
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UC_Orgument_UI(const FObjectInitializer& ObjectInitializer); // コンストラクター(本当の)
	virtual void NativeConstruct() override; // コンストラクタ override

protected:
	// ********* バインド *********
	// オーグメントカード1
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Name_1;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Type_1;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Note_1;
	UPROPERTY(meta = (BindWidget))
	UImage* Orgument_Image_Base_1;
	UPROPERTY(meta = (BindWidget))
	UImage* Orgument_Image_1;
	// オーグメントカード2
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Name_2;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Type_2;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Note_2;
	UPROPERTY(meta = (BindWidget))
	UImage* Orgument_Image_Base_2;
	UPROPERTY(meta = (BindWidget))
	UImage* Orgument_Image_2;
	// オーグメントカード3
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Name_3;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Type_3;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Note_3;
	UPROPERTY(meta = (BindWidget))
	UImage* Orgument_Image_Base_3;
	UPROPERTY(meta = (BindWidget))
	UImage* Orgument_Image_3;
	// オーグメント選択ボタン
	UPROPERTY(meta = (BindWidget))
	UButton* Orgument_Button_1;
	UPROPERTY(meta = (BindWidget))
	UButton* Orgument_Button_2;
	UPROPERTY(meta = (BindWidget))
	UButton* Orgument_Button_3;
	// ***

	// ********* Wiget関数 *********
	// オーグメント選択ボタンクリック
	UFUNCTION()
	void OrgumentButton1Clicked();
	UFUNCTION()
	void OrgumentButton2Clicked();
	UFUNCTION()
	void OrgumentButton3Clicked();
	// ***
	
	
private:
	// ステータス画像を取得
	TArray<UTexture2D*> GetStatusImage(int statusNo);
	// 戦術アイコン画像取得
	UTexture2D* GetTacticsImage(int tacticsCommandNo);

	TArray< UTextBlock*> orgumentNameTextBlanks = {}; // オーグメント名前配列
	TArray< UTextBlock*> orgumentTypeTextBlanks = {}; // オーグメント種類配列
	TArray< UTextBlock*> orgumentNoteTextBlanks = {}; // オーグメント説明配列
	TArray<int> displayOrgumentNums = {}; // 表示するオーグメントNos
	TArray<UImage*> orgumentBaseImageBlanks = {}; // オーグメントベース画像配列 (空)
	TArray<UImage*> orgumentImageBlanks = {}; // オーグメント画像配列 (空)

	// ステータスベース画像
	UTexture2D* pressStatusBaseImage = nullptr; // プレス耐性
	UTexture2D* sideStatusBaseImage = nullptr; // サイド突破
	UTexture2D* physicsStatusBaseImage = nullptr; // 走力
	// ステータス画像
	UTexture2D* pressStatusImage = nullptr; // プレス耐性
	UTexture2D* sideStatusImage = nullptr; // サイド突破
	UTexture2D* physicsStatusImage = nullptr; // 走力
	// 戦術アイコン画像
	UTexture2D* lineBreakTacticsImage = nullptr; // ラインブレイク
	UTexture2D* gkBuildUpTacticsImage = nullptr; // GKビルドアップ
	UTexture2D* highLineTacticsImage = nullptr; // ハイライン
	UTexture2D* lowBlockTacticsImage = nullptr; // ローブロック
	UTexture2D* sideAttackTacticsImage = nullptr; // サイドアタック
	UTexture2D* sideCompTacticsImage = nullptr; // サイド圧縮
};
