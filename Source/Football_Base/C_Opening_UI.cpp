// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Opening_UI.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>

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
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("Game_Field_Map")); // ゲームフィールドへ移動
}

// 終了ボタンクリック
void UC_Opening_UI::QuitButtonClicked()
{
    UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false); // ゲーム終了
}
