// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/Button.h>
#include <Components/WidgetSwitcher.h>
#include <Components/Overlay.h>
#include <Components/TextBlock.h>
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

    // ウィジェットパネル変更
    void SwitchWidgetPanal(int panalNum);
    
private:
    // エンハンスの表示・非表示
    void SwitchEnhance(int command);

    // 現在のエンハンス
    UOverlay* currentEnhance1 = nullptr;
    UOverlay* currentEnhance2 = nullptr;
    UWidgetSwitcher* currentButtonSwicher = nullptr;
    //UTextBlock* currentText = nullptr;
    //FColor textEnhanceColor = FColor::FromHex(TEXT("1996FFFF"));

protected:
        // ボタン
        UPROPERTY(meta = (BindWidget))
        UButton* Play_Button;
        // Widgetスイッチャー
        UPROPERTY(meta = (BindWidget))
        UWidgetSwitcher* UI_Switcher;
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
};
