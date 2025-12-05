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
    
private:
    // エンハンスの表示・非表示
    void SwitchEnhance(int command);

    // 現在のエンハンス
    UOverlay* currentEnhance1 = nullptr;
    UOverlay* currentEnhance2 = nullptr;
    UWidgetSwitcher* currentButtonSwicher = nullptr;
    //UTextBlock* currentText = nullptr;
    //FColor textEnhanceColor = FColor::FromHex(TEXT("1996FFFF"));
    int draftPhase = 0; // フェーズを一時保存 (変更時に)

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
        UFUNCTION()
        // ロングアタックボタンクリック
        void LongAttackButtonClicked();
        UFUNCTION()
        // レーンアタックボタンクリック
        void LaneAttackButtonClicked();
        UFUNCTION()
        // アイドルボタンクリック
        void IdleButtonClicked();
        UFUNCTION()
        // プレス回避ボタンクリック
        void EscapePressingButtonClicked();
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
