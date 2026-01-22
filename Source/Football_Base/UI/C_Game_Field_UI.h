// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/Button.h>
#include <Components/ProgressBar.h>
#include <Components/Border.h>
#include <Components/TextBlock.h>
#include <Components/Image.h>
#include "C_Game_Field_UI.generated.h"


/**
 * 
 */
UCLASS()
class FOOTBALL_BASE_API UC_Game_Field_UI : public UUserWidget
{
	GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    UC_Game_Field_UI(const FObjectInitializer& ObjectInitializer); // コンストラクター(本当の)
    virtual void NativeConstruct() override; // コンストラクタ override

protected:
    // ********* バインド *********
    // -- 試合開始ボタン --
    UPROPERTY(meta = (BindWidget))
    UButton* Match_Start_Button;
    // -- 戦術コマンド --
    UPROPERTY(meta = (BindWidget))
    UButton* Tactics_Command_Button_1;
    UPROPERTY(meta = (BindWidget))
    UImage* Tactics_Image_1;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Tactics_Command_Name_1;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Tactics_Command_Value_1;
    UPROPERTY(meta = (BindWidget))
    UBorder* Tactics_Command_Border_1;
    UPROPERTY(meta = (BindWidget))
    UButton* Tactics_Command_Button_2;
    UPROPERTY(meta = (BindWidget))
    UImage* Tactics_Image_2;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Tactics_Command_Name_2;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Tactics_Command_Value_2;
    UPROPERTY(meta = (BindWidget))
    UBorder* Tactics_Command_Border_2;
    UPROPERTY(meta = (BindWidget))
    UButton* Tactics_Command_Button_3;
    UPROPERTY(meta = (BindWidget))
    UImage* Tactics_Image_3;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Tactics_Command_Name_3;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Tactics_Command_Value_3;
    UPROPERTY(meta = (BindWidget))
    UBorder* Tactics_Command_Border_3;
    UPROPERTY(meta = (BindWidget))
    UButton* Tactics_Command_Button_4;
    UPROPERTY(meta = (BindWidget))
    UImage* Tactics_Image_4;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Tactics_Command_Name_4;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Tactics_Command_Value_4;
    UPROPERTY(meta = (BindWidget))
    UBorder* Tactics_Command_Border_4;
    // -- HPバー --
    UPROPERTY(meta = (BindWidget))
    UProgressBar* My_HP_Bar;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* My_HP_Text;
    UPROPERTY(meta = (BindWidget))
    UProgressBar* Enemy_HP_Bar;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Enemy_HP_Text;
    // -- ポイント --
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Point_1;
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Point_2;
    UPROPERTY(meta = (BindWidget))
    UBorder* Enemy_Point_1;
    UPROPERTY(meta = (BindWidget))
    UBorder* Enemy_Point_2;
    // -- 能力値 --
    // サイド突破
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Side_Break_Abilty_Box_1;
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Side_Break_Abilty_Box_2;
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Side_Break_Abilty_Box_3;
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Side_Break_Abilty_Box_4;
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Side_Break_Abilty_Box_5;
    // 走力
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Phisical_Abilty_Box_1;
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Phisical_Abilty_Box_2;
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Phisical_Abilty_Box_3;
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Phisical_Abilty_Box_4;
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Phisical_Abilty_Box_5;
    // プレス耐性
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Press_Resistance_Abilty_Box_1;
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Press_Resistance_Abilty_Box_2;
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Press_Resistance_Abilty_Box_3;
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Press_Resistance_Abilty_Box_4;
    UPROPERTY(meta = (BindWidget))
    UBorder* My_Press_Resistance_Abilty_Box_5;
    // 能力値テキスト
    UPROPERTY(meta = (BindWidget))
    UTextBlock* My_Side_Break_Abilty_Text;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* My_Phisical_Abilty_Text;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* My_Press_Resistance_Abilty_Text;
    // ラウンド
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Rount_Text;
    // ボール
    UPROPERTY(meta = (BindWidget))
    UImage* My_Ball_Image;
    UPROPERTY(meta = (BindWidget))
    UImage* Enemy_Ball_Image;
    UPROPERTY(meta = (BindWidget))
    UImage* Abilty_Erea_My_Ball_Image; // アビリティエリア画像
    // ラウンドの状態
    //UPROPERTY(meta = (BindWidget))
    //UTextBlock* Round_State_Text;
    // **********************

    // Wiget関数
    UFUNCTION()
    // ゲームスタートボタンクリック
    void MatchStartButtonClicked();
    UFUNCTION()
    // 戦術コマンド1クリック
    void TacticsCommand1Clicked();
    UFUNCTION()
    // 戦術コマンド2クリック
    void TacticsCommand2Clicked();
    UFUNCTION()
    // 戦術コマンド3クリック
    void TacticsCommand3Clicked();
    UFUNCTION()
    // 戦術コマンド4クリック
    void TacticsCommand4Clicked();
    
private:
    // 対戦処理
    void BattlePhase();
    // HP値を更新
    void UpdateHP(int damage);
    // ラウンド数更新
    void UpdateRoundNum();
    // ボール保持アイコン更新
    void UpdateBallPossessIcon();
    // ダメージ計算・取得
    // | 返り値 = +: 敵へダメージ、-: プレイヤーへダメージ |
    // | **ステータス反映未 |
    //int GetDanage();
    // 敵の戦術コマンド更新
    void UpdateEnemyTacticsCommand();
    // 戦術コマンドの使用制限(ボール保持・非保持)を更新
    void UpdateEnableTacticsCommand();
    // 戦術アイコン画像取得
    UTexture2D* GetTacticsImage(int tacticsCommandNo);

    int myPoint = 0; // 自分のポイント数
    int enemyPoint = 0; // 敵のポイント数
    int round = 0; // ラウンド数
    const FString ROUND_TOP_TEXT = "ROUND"; // ラウンドテキストの先頭
    bool isMyBall = true; // マイポールか
    int selectPlayerTacticsCommand = 0; // 選択した戦術コマンド
    const int NONE_TACTICS_COMMAND_SCORE = 20; // 戦術コマンドなしのダメージスコア
    // コマンド
    TArray< UTextBlock*> tacticsCommandNameBlanks = {}; // 戦術カード名前UI配列
    TArray< UTextBlock*> tacticsCommandScoreBlanks = {}; // 戦術カードスコアUI配列
    TArray< UImage*> tacticsImageBlanks = {}; // 戦術カード画像UI配列
    TArray< UBorder*> tacticsCommandBorders = {}; // 戦術カードボーダー配列 (*表示・非表示のみ)
    TArray< UButton*> tacticsCommandButtons = {}; // 戦術カードボタン配列 (*表示・非表示のみ)
    const FString HP_END_TEXT = "/100"; // HPテキストの末尾
    const FLinearColor POINT_COLOR = FLinearColor(FColor::FromHex("D2D2D2FF")); // ポイントの色
    // 能力の色
    const FLinearColor DEFAULT_ABILTY_COLOR = FLinearColor(FColor::FromHex("283B45FF")); // デフォルト (能力なし)
    const FLinearColor SIDE_BREAK_ABILTY_COLOR = FLinearColor(FColor::FromHex("EDBD69FF")); // サイド突破力 (ステイタスバー)
    const FLinearColor PHISICAL_ABILTY_COLOR = FLinearColor(FColor::FromHex("63E2FDFF")); // 走力 (ステイタスバー)
    const FLinearColor PRESS_RESISTANCE_ABILTY_COLOR = FLinearColor(FColor::FromHex("7366F0FF")); //プレス耐性 (ステイタスバー)
    const FString ABILTY_END_TEXT = "/5"; // 能力テキストの末尾
    const FLinearColor SIDE_BREAK_BORDER_COLOR = FLinearColor(FColor::FromHex("8C6F3EFF")); // サイド突破力(戦術コマンドボーダー)
    const FLinearColor PHISICAL_BORDER_COLOR = FLinearColor(FColor::FromHex("3B8D9EFF")); // 走力 (戦術コマンドボーダー)
    const FLinearColor PRESS_RESISTANCE_BORDER_COLOR = FLinearColor(FColor::FromHex("713FD7FF")); // プレス耐性 (戦術コマンドボーダー)
    const FLinearColor DISABLE_BORDER_TEXT_COLOR = FLinearColor(FColor::FromHex("A5A5A5FF")); // コマンド使用不可 (戦術コマンドボーダー)
    // 敵のコマンド・ステータス
    int enemyTacticsCommand = 0; // 戦術コマンド
    int enemySideBreakAbilty = 1; // サイド突破
    int enemyPhisicalAbilty = 1; // 走力
    int enemyPressResistanceAbilty = 1; // プレス耐性
    // 戦術アイコン画像
    UTexture2D* lineBreakTacticsImage = nullptr; // ラインブレイク
    UTexture2D* gkBuildUpTacticsImage = nullptr; // GKビルドアップ
    UTexture2D* highLineTacticsImage = nullptr; // ハイライン
    UTexture2D* lowBlockTacticsImage = nullptr; // ローブロック
    UTexture2D* sideAttackTacticsImage = nullptr; // サイドアタック
    UTexture2D* sideCompTacticsImage = nullptr; // サイド圧縮
};
