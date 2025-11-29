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
    if (Match_Start_Button) // 試合開始ボタン
    {
        Match_Start_Button->OnClicked.AddUniqueDynamic(this, &UC_Opening_UI::MatchStartButtonClicked);
    }
    if (Long_Attack_Not_Press_Button) // ロングアタックボタン
    {
        Long_Attack_Not_Press_Button->OnClicked.AddUniqueDynamic(this, &UC_Opening_UI::LongAttackButtonClicked);
    }
    if (Lane_Attack_Not_Press_Button) // レーンアタックボタン
    {
        Lane_Attack_Not_Press_Button->OnClicked.AddUniqueDynamic(this, &UC_Opening_UI::LaneAttackButtonClicked);
    }
    if (Idle_Not_Press_Button) // アイドルボタン
    {
        Idle_Not_Press_Button->OnClicked.AddUniqueDynamic(this, &UC_Opening_UI::IdleButtonClicked);
    }
    if (Escape_Pressing_Not_Press_Button) // プレス回避ボタン
    {
        Escape_Pressing_Not_Press_Button->OnClicked.AddUniqueDynamic(this, &UC_Opening_UI::EscapePressingButtonClicked);
    }
}

void UC_Opening_UI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (_instance == nullptr)  return;
    
    // *** フェーズ変更時、一度のみ処理 ***
    if (_instance->game_phase != draftPhase) {
        if (_instance->game_phase == C_Common::PLAYER_SELECT_PLACE_PHASE) {
            Round_State_Text->SetText(FText::FromString(TEXT("準備中")));
            Round_State_Text->SetColorAndOpacity(PLAYER_SELECT_PLACE_COLOR);
        }
        if (_instance->game_phase == C_Common::MATCH_READY_PHASE) {
            Round_State_Text->SetText(FText::FromString(TEXT("スタンバイ")));
            Round_State_Text->SetColorAndOpacity(MATCH_READY_STATE_COLOR);
        }
        if (_instance->game_phase == C_Common::MATCH_PHASE) {
            Round_State_Text->SetText(FText::FromString(TEXT("試合中")));
            Round_State_Text->SetColorAndOpacity(MATCH_STATE_COLOR);
        }
        
        draftPhase = _instance->game_phase;
    }
    // ***

    // *** タイマー ***
    if (_instance->game_phase == C_Common::MATCH_READY_PHASE || _instance->game_phase == C_Common::MATCH_PHASE) { // *(制限)試合準備・試合フェーズのみ

        _instance->phase_count -= InDeltaTime; // カウントダウン
        if (_instance->phase_count < 0) return;

        Count_Text->SetText(FText::FromString(FString::FromInt(_instance->phase_count))); // テキスト更新
    }
    // ***
}

// ウィジェットパネル変更
void UC_Opening_UI::SwitchWidgetPanal(int panalNum)
{
    if (UI_Switcher == nullptr) return;

    UI_Switcher->SetActiveWidgetIndex(panalNum);
}

// ボタンパネル変更
void UC_Opening_UI::SwitchButtonPanal(int panalNum)
{
    if (Button_Switcher == nullptr) return;

    Button_Switcher->SetActiveWidgetIndex(panalNum);
}

// エンハンスの表示・非表示
void UC_Opening_UI::SwitchEnhance(int command)
{
    // エンハンス非表示
    if (currentEnhance1) {
        currentEnhance1->SetVisibility(ESlateVisibility::Hidden);
    }
    if (currentEnhance2) {
        currentEnhance2->SetVisibility(ESlateVisibility::Hidden);
    }
    if (currentButtonSwicher) {
        currentButtonSwicher->SetActiveWidgetIndex(0);
    }
    //if (currentText) {
    //    currentText->SetColorAndOpacity(FColor::White);
    //}

    // エンハンスの取得
    if (command == C_Common::ESCAPE_PRESSING_COMMAND_NO) {
        currentEnhance1 = Escape_Press_Enhance_1;
        currentEnhance2 = Escape_Press_Enhance_2;
        currentButtonSwicher = Escape_Pressing_Switcher;
        //currentText = Escape_Press_Text;
    }
    else if (command == C_Common::LONG_ATTACK_COMMAND_NO) {
        currentEnhance1 = Long_Attack_Enhance_1;
        currentEnhance2 = Long_Attack_Enhance_2;
        currentButtonSwicher = Long_Attack_Switcher;
        //currentText = Long_Attack_Text;
    }
    else if (command == C_Common::LANE_ATTACK_COMMAND_NO) {
        currentEnhance1 = Lane_Attack_Enhance_1;
        currentEnhance2 = Lane_Attack_Enhance_2;
        currentButtonSwicher = Lane_Attack_Switcher;
        //currentText = Lane_Attack_Text;
    }
    else {
        currentEnhance1 = Idle_Enhance_1;
        currentEnhance2 = Idle_Enhance_2;
        currentButtonSwicher = Idle_Switcher;
        //currentText = Idle_Text;
    }

    // エンハンス表示
    if (currentEnhance1) {
        currentEnhance1->SetVisibility(ESlateVisibility::Visible);
    }
    if (currentEnhance2) {
        currentEnhance2->SetVisibility(ESlateVisibility::Visible);
    }
    if (currentButtonSwicher) {
        currentButtonSwicher->SetActiveWidgetIndex(1);
    }
    //if (currentText) {
    //    currentText->SetColorAndOpacity(textEnhanceColor);
    //}
}

// プレイボタンクリック
void UC_Opening_UI::PlayButtonClicked()
{
    // ●マネージャー選択フェーズへ
    // UI変更
    SwitchWidgetPanal(C_Common::MANAGER_SELECT_BEFORE_PHASE);
    // フェーズ変更
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (_instance == nullptr) return;

    _instance->game_phase = C_Common::MANAGER_SELECT_BEFORE_PHASE; // フェーズ変更
    
}

// 終了ボタンクリック
void UC_Opening_UI::QuitButtonClicked()
{
    UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false); // ゲーム終了
}

// 試合開始ボタンクリック
void UC_Opening_UI::MatchStartButtonClicked()
{
    // フェーズ変更
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (_instance == nullptr) return;

    SwitchButtonPanal(1); // ボタン変更(保持コマンド)
    _instance->game_phase = C_Common::MATCH_READY_PHASE; // (試合準備)フェーズ変更
    SwitchEnhance(_instance->command); // ボタンエンハンス設定
    _instance->phase_count = C_Common::MATCH_READY_TIME; // 試合準備フェーズカウントセット
}

// ロングアタックボタンクリック
void UC_Opening_UI::LongAttackButtonClicked()
{
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (_instance == nullptr)  return;
    if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

    _instance->command = C_Common::LONG_ATTACK_COMMAND_NO;
    SwitchEnhance(_instance->command); // エンハンス表示
}

// レーンアタックボタンクリック
void UC_Opening_UI::LaneAttackButtonClicked()
{
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (_instance == nullptr)  return;
    if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

    _instance->command = C_Common::LANE_ATTACK_COMMAND_NO;
    SwitchEnhance(_instance->command); // エンハンス表示
}

// アイドルボタンクリック
void UC_Opening_UI::IdleButtonClicked()
{
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (_instance == nullptr)  return;
    if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

    _instance->command = 0;
    SwitchEnhance(_instance->command); // エンハンス表示
}

// プレス回避ボタンクリック
void UC_Opening_UI::EscapePressingButtonClicked()
{
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (_instance == nullptr)  return;
    if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

    _instance->command = C_Common::ESCAPE_PRESSING_COMMAND_NO;
    SwitchEnhance(_instance->command); // エンハンス表示
}
