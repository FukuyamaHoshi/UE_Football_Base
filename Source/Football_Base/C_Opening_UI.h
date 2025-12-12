// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/Button.h>
#include <Components/WidgetSwitcher.h>
#include <Components/Overlay.h>
#include <Components/TextBlock.h>
#include <Components/VerticalBox.h>
#include "C_Opening_UI.generated.h"


/**
 * 
 */
UCLASS()
class FOOTBALL_BASE_API UC_Opening_UI : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override; // コンストラクタ override
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override; // Tick

    // ウィジェットパネル変更
    void SwitchWidgetPanal(int panalNum);
    // ボタン変更
    void SwitchButtonPanal(int panalNum);
    // 保持・非保持ラベル変更
    void SwitchHasLabelPanal(int panalNum);
    // 保持・非保持ラベル表示・非表示切替
    void SetVisibleHasLabel(bool isVisibleLabel);
    
private:
    // エンハンスの表示・非表示
    void SwitchEnhance(int command, bool isHasBall = true);

    // 現在のエンハンス
    UOverlay* currentEnhance1 = nullptr;
    UOverlay* currentEnhance2 = nullptr;
    UWidgetSwitcher* currentButtonSwicher = nullptr;
    //UTextBlock* currentText = nullptr;
    //FColor textEnhanceColor = FColor::FromHex(TEXT("1996FFFF"));
    int draftPhase = 0; // フェーズを一時保存 (変更時に)
    int draftCommand = 0; // コマンドを一時保存 (変更時に)

    const FLinearColor PLAYER_SELECT_PLACE_COLOR = FLinearColor(FColor::FromHex("22C832FF")); // Stateテキストの色(緑)
    const FLinearColor MATCH_READY_STATE_COLOR = FLinearColor(FColor::FromHex("FFDA00FF")); // Stateテキストの色(黄色)
    const FLinearColor MATCH_STATE_COLOR = FLinearColor(FColor::FromHex("D31F27FF")); // Stateテキストの色(赤)

protected:
        // ボタン
        UPROPERTY(meta = (BindWidget))
        UButton* Play_Button;
        // Widgetスイッチャー
        UPROPERTY(meta = (BindWidget))
        UWidgetSwitcher* UI_Switcher;
        UPROPERTY(meta = (BindWidget))
        UWidgetSwitcher* Button_Switcher;
        UPROPERTY(meta = (BindWidget))
        UButton* Quit_Button;
    
        UPROPERTY(meta = (BindWidget))
        UButton* Match_Start_Button;
        
        // *** 試合フェーズ ***
        // -- 保持コマンド --
        UPROPERTY(meta = (BindWidget))
        UButton* Long_Attack_Not_Press_Button;
        UPROPERTY(meta = (BindWidget))
        UButton* Lane_Attack_Not_Press_Button;
        UPROPERTY(meta = (BindWidget))
        UButton* Idle_Not_Press_Button;
        UPROPERTY(meta = (BindWidget))
        UButton* Escape_Pressing_Not_Press_Button;
        UPROPERTY(meta = (BindWidget))
        UWidgetSwitcher* Long_Attack_Switcher;
        UPROPERTY(meta = (BindWidget))
        UWidgetSwitcher* Lane_Attack_Switcher;
        UPROPERTY(meta = (BindWidget))
        UWidgetSwitcher* Idle_Switcher;
        UPROPERTY(meta = (BindWidget))
        UWidgetSwitcher* Escape_Pressing_Switcher;
        // -- 非保持コマンド --
        UPROPERTY(meta = (BindWidget))
        UButton* Side_Press_Not_Press_Button;
        UPROPERTY(meta = (BindWidget))
        UButton* High_Press_Not_Press_Button;
        UPROPERTY(meta = (BindWidget))
        UButton* No_Command_Not_Press_Button;
        UPROPERTY(meta = (BindWidget))
        UButton* Low_Block_Not_Press_Button;
        UPROPERTY(meta = (BindWidget))
        UWidgetSwitcher* Side_Press_Switcher;
        UPROPERTY(meta = (BindWidget))
        UWidgetSwitcher* High_Press_Switcher;
        UPROPERTY(meta = (BindWidget))
        UWidgetSwitcher* No_Command_Switcher;
        UPROPERTY(meta = (BindWidget))
        UWidgetSwitcher* Low_Block_Switcher;
        
        // エンハンス
        // -- ロングアタック --
        UPROPERTY(meta = (BindWidget))
        UOverlay* Long_Attack_Enhance_1;
        UPROPERTY(meta = (BindWidget))
        UOverlay* Long_Attack_Enhance_2;
        UPROPERTY(meta = (BindWidget))
        UTextBlock* Long_Attack_Text;
        // -- プレス回避 --
        UPROPERTY(meta = (BindWidget))
        UOverlay* Escape_Press_Enhance_1;
        UPROPERTY(meta = (BindWidget))
        UOverlay* Escape_Press_Enhance_2;
        UPROPERTY(meta = (BindWidget))
        UTextBlock* Escape_Press_Text;
        // -- アイドル --
        UPROPERTY(meta = (BindWidget))
        UOverlay* Idle_Enhance_1;
        UPROPERTY(meta = (BindWidget))
        UOverlay* Idle_Enhance_2;
        UPROPERTY(meta = (BindWidget))
        UTextBlock* Idle_Text;
        // -- レーンアタック --
        UPROPERTY(meta = (BindWidget))
        UOverlay* Lane_Attack_Enhance_1;
        UPROPERTY(meta = (BindWidget))
        UOverlay* Lane_Attack_Enhance_2;
        UPROPERTY(meta = (BindWidget))
        UTextBlock* Lane_Attack_Text;
        // -- サイド圧縮 --
        UPROPERTY(meta = (BindWidget))
        UOverlay* Side_Press_Enhance_1;
        UPROPERTY(meta = (BindWidget))
        UOverlay* Side_Press_Enhance_2;
        UPROPERTY(meta = (BindWidget))
        UTextBlock* Side_Press_Text;
        // -- ハイプレス --
        UPROPERTY(meta = (BindWidget))
        UOverlay* High_Press_Enhance_1;
        UPROPERTY(meta = (BindWidget))
        UOverlay* High_Press_Enhance_2;
        UPROPERTY(meta = (BindWidget))
        UTextBlock* High_Press_Text;
        // -- ノーコマンド --
        UPROPERTY(meta = (BindWidget))
        UOverlay* No_Command_Enhance_1;
        UPROPERTY(meta = (BindWidget))
        UOverlay* No_Command_Enhance_2;
        UPROPERTY(meta = (BindWidget))
        UTextBlock* No_Command_Text;
        // -- ローブロック --
        UPROPERTY(meta = (BindWidget))
        UOverlay* Low_Block_Enhance_1;
        UPROPERTY(meta = (BindWidget))
        UOverlay* Low_Block_Enhance_2;
        UPROPERTY(meta = (BindWidget))
        UTextBlock* Low_Block_Text;
        // -- 保持・非保持ラベルSwicher --
        UPROPERTY(meta = (BindWidget))
        UWidgetSwitcher* Is_Has_Label_Swicher;
        // ***

        // *** プレイヤー選択フェーズ ***
        // -- ボタン ---
        UPROPERTY(meta = (BindWidget))
        UButton* Player_Select_Button_0;
        UPROPERTY(meta = (BindWidget))
        UButton* Player_Select_Button_1;
        UPROPERTY(meta = (BindWidget))
        UButton* Player_Select_Button_2;
        UPROPERTY(meta = (BindWidget))
        UButton* Player_Select_Button_3;
        UPROPERTY(meta = (BindWidget))
        UButton* Player_Select_Button_4;
        // -- ステータスボックス --
        UPROPERTY(meta = (BindWidget))
        UVerticalBox* Player_Status_Box_0;
        UPROPERTY(meta = (BindWidget))
        UVerticalBox* Player_Status_Box_1;
        UPROPERTY(meta = (BindWidget))
        UVerticalBox* Player_Status_Box_2;
        UPROPERTY(meta = (BindWidget))
        UVerticalBox* Player_Status_Box_3;
        UPROPERTY(meta = (BindWidget))
        UVerticalBox* Player_Status_Box_4;
        // ***
        
        // *** カウントダウン ***
        UPROPERTY(meta = (BindWidget))
        UTextBlock* Count_Text;
        // ***

         // *** ラウンドの状態 ***
        UPROPERTY(meta = (BindWidget))
        UTextBlock* Round_State_Text;
        // ***

        // プレイボタンクリック
        UFUNCTION()
        void PlayButtonClicked();
        // 終了ボタンクリック
        UFUNCTION()
        void QuitButtonClicked();
        // 試合開始ボタンクリック
        UFUNCTION()
        void MatchStartButtonClicked();

        // *** 試合フェーズ ***
        // -- 保持コマンド --
        UFUNCTION()
        // ロングアタックボタンクリック
        void LongAttackButtonClicked();
        UFUNCTION()
        // テクニカルアタックボタンクリック
        void TacnicalAttackButtonClicked();
        UFUNCTION()
        // アイドルボタンクリック
        void IdleButtonClicked();
        UFUNCTION()
        // ポゼッションボタンクリック
        void EscapePressingButtonClicked();
        // -- 非保持コマンド --
        UFUNCTION()
        // サイド圧縮ボタンクリック
        void SidePressButtonClicked();
        UFUNCTION()
        // ハイプレスボタンクリック
        void HighPressButtonClicked();
        UFUNCTION()
        // ノーコマンドボタンクリック
        void NoCommandButtonClicked();
        UFUNCTION()
        // ローブロックボタンクリック
        void LowBlockButtonClicked();
        // ***

        // *** プレイヤー選択フェーズ ***
        UFUNCTION()
        // プレイヤー選択ボタン (0) クリック
        void PlayerSelectButton0Clicked();
        UFUNCTION()
        // プレイヤー選択ボタン (1) クリック
        void PlayerSelectButton1Clicked();
        UFUNCTION()
        // プレイヤー選択ボタン (2) クリック
        void PlayerSelectButton2Clicked();
        UFUNCTION()
        // プレイヤー選択ボタン (3) クリック
        void PlayerSelectButton3Clicked();
        UFUNCTION()
        // プレイヤー選択ボタン (4) クリック
        void PlayerSelectButton4Clicked();
        // ***
};
