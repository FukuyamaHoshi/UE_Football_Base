// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Game_Field_UI.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "My_Game_Instance.h"
#include "C_Common.h"

void UC_Game_Field_UI::NativeConstruct()
{
    // *** クリックイベントをバインド ***
    if (Match_Start_Button) // 試合開始ボタン
    {
        Match_Start_Button->OnClicked.AddUniqueDynamic(this, &UC_Game_Field_UI::MatchStartButtonClicked);
    }
    if (Tactics_Command_Button_1) // 戦術コマンド1
    {
        Tactics_Command_Button_1->OnClicked.AddUniqueDynamic(this, &UC_Game_Field_UI::TacticsCommand1Clicked);
    }
    if (Tactics_Command_Button_2) // 戦術コマンド2
    {
        Tactics_Command_Button_2->OnClicked.AddUniqueDynamic(this, &UC_Game_Field_UI::TacticsCommand2Clicked);
    }
    if (Tactics_Command_Button_3) // 戦術コマンド3
    {
        Tactics_Command_Button_3->OnClicked.AddUniqueDynamic(this, &UC_Game_Field_UI::TacticsCommand3Clicked);
    }
    if (Tactics_Command_Button_4) // 戦術コマンド4
    {
        Tactics_Command_Button_4->OnClicked.AddUniqueDynamic(this, &UC_Game_Field_UI::TacticsCommand4Clicked);
    }
    // ***

    // *** 能力値をセット (プレイヤー) ***
    UMy_Game_Instance* instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (instance) {
        // 〇サイド突破
        TArray<UBorder*> _mySideBreakBoxes = {
            My_Side_Break_Abilty_Box_1, My_Side_Break_Abilty_Box_2, My_Side_Break_Abilty_Box_3, My_Side_Break_Abilty_Box_4, My_Side_Break_Abilty_Box_5 };
        // 色をセット
        for (int i = 0; i < instance->player_side_break_abilty; ++i) {
            _mySideBreakBoxes[i]->SetBrushColor(SIDE_BREAK_ABILTY_COLOR);
        }
        // テキストをセット
        FString _mySideBreakText = FString::FromInt(instance->player_side_break_abilty) + ABILTY_END_TEXT;
        My_Side_Break_Abilty_Text->SetText(FText::FromString(_mySideBreakText));

        // 〇走力
        TArray<UBorder*> _myPhisicalBoxes = {
            My_Phisical_Abilty_Box_1, My_Phisical_Abilty_Box_2, My_Phisical_Abilty_Box_3, My_Phisical_Abilty_Box_4, My_Phisical_Abilty_Box_5 };
        // 色をセット
        for (int i = 0; i < instance->player_physical_abilty; ++i) {
            _myPhisicalBoxes[i]->SetBrushColor(PHISICAL_ABILTY_COLOR);
        }
        // テキストをセット
        FString _myPhysicalText = FString::FromInt(instance->player_physical_abilty) + ABILTY_END_TEXT;
        My_Phisical_Abilty_Text->SetText(FText::FromString(_myPhysicalText));

        // 〇プレス耐性
        TArray<UBorder*> _myPressResistanceBoxes = {
            My_Press_Resistance_Abilty_Box_1, My_Press_Resistance_Abilty_Box_2, My_Press_Resistance_Abilty_Box_3, My_Press_Resistance_Abilty_Box_4, My_Press_Resistance_Abilty_Box_5 };
        // 色をセット
        for (int i = 0; i < instance->player_press_resistance_abilty; ++i) {
            _myPressResistanceBoxes[i]->SetBrushColor(PRESS_RESISTANCE_ABILTY_COLOR);
        }
        // テキストをセット
        FString _myPressResistanceText = FString::FromInt(instance->player_press_resistance_abilty) + ABILTY_END_TEXT;
        My_Press_Resistance_Abilty_Text->SetText(FText::FromString(_myPressResistanceText));
    }
    // ***

    // *** ラウンド数更新 ***
    UpdateRoundNum();
    // ***

    // *** ボール保持アイコン更新 ***
    UpdateBallPossessIcon();
    // ***

    // *** 戦術コマンドカードセット ***
    if (instance) {
        if (instance->players_tactics_command_nums.Num() > 0) { // オーグメントがある場合
            // 戦術カード名前UI配列作成
            tacticsCommandNameBlanks = { Tactics_Command_Name_1, Tactics_Command_Name_2, Tactics_Command_Name_3, Tactics_Command_Name_4 };

            for (int _i = 0; _i < instance->players_tactics_command_nums.Num(); _i++)
            {
                TArray<FString> _texts = C_Common::GetTacticsCommandTexts(instance->players_tactics_command_nums[_i]); // オーグメントテキスト
                tacticsCommandNameBlanks[_i]->SetText(FText::FromString(_texts[0])); // *名前セット
            }
        }
    }
    // ***
}

// 試合開始ボタンクリック
void UC_Game_Field_UI::MatchStartButtonClicked()
{
}

// 戦術コマンド1クリック
void UC_Game_Field_UI::TacticsCommand1Clicked()
{
    UKismetSystemLibrary::PrintString(this, "1", true, true, FColor::Cyan, 2.f, TEXT("None"));
    
    // *** 対戦処理 ***
    BattlePhase();
    // ***
}

// 戦術コマンド2クリック
void UC_Game_Field_UI::TacticsCommand2Clicked()
{
    UKismetSystemLibrary::PrintString(this, "tactick 2 button", true, true, FColor::Cyan, 2.f, TEXT("None"));

    UGameplayStatics::OpenLevel(GetWorld(), TEXT("Route")); // ルートMAPへ(仮)

    // *** 試合ラウンド(決勝とかの)セット ***
    UMy_Game_Instance* instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (instance)
    {
        instance->match_round++; // 次のラウンド
    }
    // ***
}

// 戦術コマンド3クリック
void UC_Game_Field_UI::TacticsCommand3Clicked()
{
    UKismetSystemLibrary::PrintString(this, "3", true, true, FColor::Cyan, 2.f, TEXT("None"));
}

// 戦術コマンド4クリック
void UC_Game_Field_UI::TacticsCommand4Clicked()
{
    UKismetSystemLibrary::PrintString(this, "4", true, true, FColor::Cyan, 2.f, TEXT("None"));
}

// 対戦処理
void UC_Game_Field_UI::BattlePhase()
{
    // *** ダメージ取得 ***
    int _damage = GetDanage();
    // ***
    
    // *** HP更新 ***
    UpdateHP(_damage);
    // ***

    // *** ラウンド更新時、処理 ***
    //  →HPが0になれば発火
    int _myHp = My_HP_Bar->GetPercent() * 100; // myHP
    if (_myHp <= 0) { // プレイヤーHPが0の場合、

        // * ポイント更新 (敵側の) *
        // 〇数更新
        enemyPoint++;
        // 〇色セット
        if (enemyPoint <= 1) { // ポイント1以下か
            // Point 1
            Enemy_Point_1->SetBrushColor(POINT_COLOR);
        }
        else {
            // point 2
            Enemy_Point_2->SetBrushColor(POINT_COLOR);
        }
        // *

        // * 試合終了時処理 *
        if (enemyPoint >= 2) {
            UGameplayStatics::OpenLevel(GetWorld(), TEXT("Opening")); // オープニングMAPへ

            return;
        }
        // *

        // * HPリセット (プレイヤー側の) *
        My_HP_Bar->SetPercent(1.0f); // HP値
        FString _myHpText = "100" + HP_END_TEXT;
        My_HP_Text->SetText(FText::FromString(_myHpText)); // HPテキスト
        // *

        // * ボール保持者・アイコン更新 *
        isMyBall = !isMyBall;
        UpdateBallPossessIcon();
        // *

        // * ラウンド数更新 *
        UpdateRoundNum();
        // *
    }

    int _enemyHp = Enemy_HP_Bar->GetPercent() * 100; // 敵HP
    if (_enemyHp <= 0) { // 敵HPが0の場合、

        //  * ポイント更新 (プレイヤー側の) *
        // 〇ポイント数更新
        myPoint++;
        // 〇色セット
        if (myPoint <= 1) { // ポイント1以下か
            // Point 1
            My_Point_1->SetBrushColor(POINT_COLOR);
        }
        else {
            // point 2
            My_Point_2->SetBrushColor(POINT_COLOR);
        }
        // *

        //  * 試合終了時処理 *
        if (myPoint >= 2) {
            UGameplayStatics::OpenLevel(GetWorld(), TEXT("Route")); // ルートMAPへ

            return;
        }
        // *

        // * HPリセット (敵側の) *
        Enemy_HP_Bar->SetPercent(1.0f); // HP値
        FString _enemyHpText = "100" + HP_END_TEXT;
        Enemy_HP_Text->SetText(FText::FromString(_enemyHpText)); // HPテキスト
        // *

        // * ボール保持者・アイコン更新 *
        isMyBall = !isMyBall;
        UpdateBallPossessIcon();
        // *

        // * ラウンド数更新 *
        UpdateRoundNum();
        // *
    }
}

// HP値を更新
void UC_Game_Field_UI::UpdateHP(int damage)
{
    // 更新するHPバー取得
    UProgressBar* _HPBar = nullptr; // HPバー
    UTextBlock* _HPText = nullptr; // HPテキスト
    if (damage > 0) { // どっちのダメージ量が多いか
        // 敵にダメージを与える
        _HPBar = Enemy_HP_Bar;
        _HPText = Enemy_HP_Text;
    }
    else {
        // 自分がダメージを受ける
        _HPBar = My_HP_Bar;
        _HPText = My_HP_Text;
    }
    // ***
    
    // *** HP値セット ***
    int _currentHp = _HPBar->GetPercent() * 100; // 現在のHP
    // HP値を更新
    damage = FMath::Abs(damage); // 正の数へ変換
    int _newHp = _currentHp - damage; // 更新後HP
    // HP値をセット
    float f_newHp = float(_newHp) / 100; // floatへ変換
    _HPBar->SetPercent(f_newHp);
    // ***

    // *** HPテキストセット ***
    FString _HPTextValue = FString::FromInt(_newHp) + HP_END_TEXT;
    _HPText->SetText(FText::FromString(_HPTextValue)); // テキスト更新
    // ***
}

// ラウンド数更新・セット
void UC_Game_Field_UI::UpdateRoundNum()
{
    // -- 更新 --
    round++;

    // -- セット --
    FString _roundText = ROUND_TOP_TEXT + FString::FromInt(round);
    Rount_Text->SetText(FText::FromString(_roundText));
}

// ボール保持アイコン更新
void UC_Game_Field_UI::UpdateBallPossessIcon()
{
    if (isMyBall) {
        // プレイヤー
        My_Ball_Image->SetOpacity(1.0f);
        Enemy_Ball_Image->SetOpacity(0);
        Abilty_Erea_My_Ball_Image->SetOpacity(1.0f);
    }
    else {
        // 敵
        My_Ball_Image->SetOpacity(0);
        Enemy_Ball_Image->SetOpacity(1.0f);
        Abilty_Erea_My_Ball_Image->SetOpacity(0);
    }
}

// ダメージ計算・取得
// | ステータス反映未 |
int UC_Game_Field_UI::GetDanage()
{
    const int MY_COMMAND_DAMAGE = 30; // *暫定 (固定値にする)
    const int ENEMY_COMMAND_DAMAGE = 30; // *暫定 (固定値にする)

    // *** ダメージ計算 ***
    int _myDamage = MY_COMMAND_DAMAGE; // 自分のダメージ量
    int _enemyDamage = ENEMY_COMMAND_DAMAGE * 3; // 相手のダメージ量
    int _damage = _myDamage - _enemyDamage; // ダメージ計算
    
    
    return _damage;
}
