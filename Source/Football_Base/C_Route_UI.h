// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/Button.h>
#include <Components/Border.h>
#include <Components/TextBlock.h>
#include <Components/RetainerBox.h>
#include "C_Route_UI.generated.h"


/**
 * 
 */
UCLASS()
class FOOTBALL_BASE_API UC_Route_UI : public UUserWidget
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UC_Route_UI(const FObjectInitializer& ObjectInitializer); // コンストラクター(本当の)
	virtual void NativeConstruct() override; // コンストラクタ override

protected:
	// ********* バインド *********
	// -- 次の試合ボタン --
	UPROPERTY(meta = (BindWidget))
	UButton* Next_Match_Button = nullptr;
	// -- 試合結果(チーム名ボックス、チーム名テキスト,得点テキスト) --
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_1;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_1;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_1;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_1;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_2;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_2;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_2;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_2;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_3;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_3;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_3;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_3;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_4;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_4;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_4;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_4;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_5;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_5;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_5;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_5;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_6;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_6;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_6;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_6;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_7;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_7;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_7;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_7;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_8;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_8;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_8;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_8;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_9;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_9;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_9;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_9;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_10;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_10;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_10;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_10;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_11;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_11;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_11;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_11;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_12;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_12;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_12;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_12;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_13;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_13;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_13;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_13;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_14;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_14;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_14;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_14;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_15;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_15;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_15;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_15;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_16;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_16;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_16;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_16;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_17;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_17;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_17;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_17;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_18;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_18;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_18;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_18;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_19;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_19;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_19;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_19;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_20;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_20;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_20;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_20;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_21;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_21;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_21;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_21;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_22;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_22;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_22;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_22;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_23;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_23;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_23;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_23;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_24;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_24;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_24;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_24;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_25;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_25;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_25;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_25;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_26;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_26;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_26;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_26;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_27;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_27;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_27;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_27;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_28;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_28;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_28;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_28;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_29;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_29;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_29;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_29;
	// ---
	UPROPERTY(meta = (BindWidget))
	UBorder* Team_Box_30;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Team_Text_30;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Score_Text_30;
	UPROPERTY(meta = (BindWidget))
	URetainerBox* RetainerBox_30;

	// ********* Wiget関数 *********
	UFUNCTION()
	// 次の試合ボタンクリック
	void NextMatchButtonClicked();

	private:
		// 対戦情報を表示する
		void DisplayMatchInfo();
		// UIにセット
		void SetInfoUI();

		TArray<UBorder*> teamBoxes = {}; // チームボックス配列
		TArray<UTextBlock*> teamTextBlanks = {}; // チームテキスト(空欄)配列
		TArray<FString> teamNames = {}; // チーム名前配列
		TArray<UTextBlock*> scoreBlanks = {}; // スコア配列
		TArray< URetainerBox*> retainerBoxes = {}; // 発光ボックス配列 (プレイヤー)
		UMaterialInstance* glowContstantMaterial = nullptr; // プレイヤー発光マテリアル

		// 色
		const FLinearColor MY_TEAM_BOX_COLOR = FLinearColor(FColor::FromHex("3FFF68")); // 自分のチームボックスの色
		const FLinearColor TEAM_BOX_WIN_COLOR = FLinearColor(FColor::FromHex("FFFF01FF")); // チームボックス勝利時の色
		const FLinearColor TEAM_BOX_LOSE_COLOR = FLinearColor::Black; // チームボックス敗戦時の色
		const FLinearColor TEAM_BOX_MATCH_COLOR = FLinearColor(FColor::FromHex("E7E7E7")); // チームボックス対戦時の色
		const FLinearColor TEAM_TEXT_LOSE_COLOR = FLinearColor(FColor::FromHex("7B7B7BFF")); // チームテキスト敗戦時の色

		// チーム名
		const FString MY_TEAM_NAME = "hoshi157"; // プレイヤーチーム
		const FString ENEMY_TEAM_NAME_1 = "1";
		const FString ENEMY_TEAM_NAME_2 = "2";
		const FString ENEMY_TEAM_NAME_3 = "3";
		const FString ENEMY_TEAM_NAME_4 = "4";
		const FString ENEMY_TEAM_NAME_5 = "5";
		const FString ENEMY_TEAM_NAME_6 = "6";
		const FString ENEMY_TEAM_NAME_7 = "7";
		const FString ENEMY_TEAM_NAME_8 = "8";
		const FString ENEMY_TEAM_NAME_9 = "9";
		const FString ENEMY_TEAM_NAME_10 = "10";
		const FString ENEMY_TEAM_NAME_11 = "11";
		const FString ENEMY_TEAM_NAME_12 = "12";
		const FString ENEMY_TEAM_NAME_13 = "13";
		const FString ENEMY_TEAM_NAME_14 = "14";
		const FString ENEMY_TEAM_NAME_15 = "15";

		const TArray<int> PLAYER_BOX_NUMS = {2, 17, 25, 29}; // プレイヤーボックスNo
};
