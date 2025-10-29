// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Opening_UI.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "My_Game_Instance.h"
#include "C_Common.h"

// コンストラクタ
void UC_Opening_UI::NativeConstruct()
{
    Super::NativeConstruct();

    // クリックイベントをバインド
    if (Play_Button) // プレイボタン
    {
        Play_Button->OnClicked.AddUniqueDynamic(this, &UC_Opening_UI::PlayButtonClicked);
    }
    if (Quit_Button) // 終了ボタン
    {
        Quit_Button->OnClicked.AddUniqueDynamic(this, &UC_Opening_UI::QuitButtonClicked);
    }
}

// ゲームスタートボタンクリック
void UC_Opening_UI::PlayButtonClicked()
{
    // ●マネージャー選択フェーズへ
    // UI変更
    UI_Switcher->SetActiveWidgetIndex(C_Common::MANAGER_SELECT_PHASE);
    // フェーズ変更
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (_instance) _instance->game_phase = C_Common::MANAGER_SELECT_PHASE;
}

// 終了ボタンクリック
void UC_Opening_UI::QuitButtonClicked()
{
    UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false); // ゲーム終了
}
