// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/Button.h>
#include <Components/WidgetSwitcher.h>
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

protected:
    // ボタン
    UPROPERTY(meta = (BindWidget))
        UButton* Play_Button;

    UPROPERTY(meta = (BindWidget))
        UButton* Quit_Button;
    // Widgetスイッチャー
    UPROPERTY(meta = (BindWidget))
        UWidgetSwitcher* UI_Switcher;
    UPROPERTY(meta = (BindWidget))
        UButton* Match_Start_Button;

    UFUNCTION()
        // ゲームスタートボタンクリック
        void PlayButtonClicked();
    UFUNCTION()
        // 終了ボタンクリック
        void QuitButtonClicked();
};
