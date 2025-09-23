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
    UButton* Tactics_Command_Button_2;
    UPROPERTY(meta = (BindWidget))
    UButton* Tactics_Command_Button_3;
    UPROPERTY(meta = (BindWidget))
    UButton* Tactics_Command_Button_4;
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
    int GetDanage();

    int myPoint = 0; // 自分のポイント数
    int enemyPoint = 0; // 敵のポイント数
    int round = 0; // ラウンド数
    const FString ROUND_TOP_TEXT = "ROUND"; // ラウンドテキストの先頭
    bool isMyBall = true; // マイポールか
    // プレイヤー能力
    int mySideBreakAbilty = 2; // サイド突破力
    int myPhysicalAbilty = 3; // 走力
    int myPressResistanceAbilty = 2; // プレス耐性
    const FString HP_END_TEXT = "/100"; // HPテキストの末尾
    const FLinearColor POINT_COLOR = FLinearColor(FColor::FromHex("D2D2D2FF")); // ポイントの色
    // 能力の色
    const FLinearColor DEFAULT_ABILTY_COLOR = FLinearColor(FColor::FromHex("283B45FF")); // デフォルト (能力なし)
    const FLinearColor SIDE_BREAK_ABILTY_COLOR = FLinearColor(FColor::FromHex("EDBD69FF")); // サイド突破力
    const FLinearColor PHISICAL_ABILTY_COLOR = FLinearColor(FColor::FromHex("63E2FDFF")); // 走力
    const FLinearColor PRESS_RESISTANCE_ABILTY_COLOR = FLinearColor(FColor::FromHex("7366F0FF")); //プレス耐性
    const FString ABILTY_END_TEXT = "/5"; // 能力テキストの末尾
};
