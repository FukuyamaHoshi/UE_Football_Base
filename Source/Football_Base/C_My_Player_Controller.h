// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_Tile.h"
#include "C_Ball.h"
#include "C_Piece.h"
#include "GameFramework/PlayerController.h"
#include "C_My_Player_Controller.generated.h"

/**
 * 
 */
UCLASS()
class FOOTBALL_BASE_API AC_My_Player_Controller : public APlayerController
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// ********* input *********
	// 入力設定
	void SetupInput();
	// 左クリック(プレス)イベント
	// | プレイヤー選択 |
	void PressedLeft();
	// 左クリック(リリース)イベント
	void ReleasedLeft();
	// (*デバッグ時のみ)スペースキー(プレス)イベント
	void PressedSpaceBar();
	// マウスホバー時,タイルをハイライトする
	void TileHighlightWhenMouseHover();
	// **********************
	


	// ********* プレイ判断 *********
	// ボールホルダーのプレイ選択
	void SelectPlayForBallHolder();
	// ディフェンダーのプレイ選択
	void SelectPlayForDefender(AC_Piece* defensePlayer);
	// ゴールキーパーのプレイ選択
	void SelectPlayForGoalKeeper();
	// *****************************
	
	
	
	// ********* プレイ *********
	// ショートパス
	// ( 引数: targetPiece(コマ) )
	void ShortPass(AC_Piece* targetPiece);
	// ロングパス
	// ( 引数: targetPiece(コマ) )
	void LongPass(AC_Piece* targetPiece);
	// ロングアタック
	void LongAttack();
	// ロングボールの事前動作
	void PreActionForLongBall();
	// ドリブル (前進のみ)
	void Drrible();
	// 方向転換 
	void ChangeOfDirection();
	// シュート
	void Shoot(AC_Piece* targetPlayer = nullptr);
	// 対人
	// | dueledPlayer: 対人をされるプレイヤー (発火時,ディフェンダー) |
	// | return bool: デゥエルの勝者(ボールホルダー = ture, ディフェンダー = false) |
	bool Duel(AC_Piece* dueledPlayer);
	// 背面対人 (背負った時のプレー)
	// | dueledPlayer: 対人をされるプレイヤー (発火時,ディフェンダー) |
	// | return bool: デゥエルの勝者(ボールホルダー = ture, ディフェンダー = false) |
	bool BackDuel(AC_Piece* dueledPlayer);
	// サイド対人
	// | dueledPlayer: 対人をされるプレイヤー (発火時,ディフェンダー) |
	// | return bool: デゥエルの勝者(ボールホルダー = ture, ディフェンダー = false) |
	bool SideDuel(AC_Piece* dueledPlayer);
	// エアバトル
	// | dueledPlayer: 対人をされるプレイヤー (発火時,ディフェンダー) |
	// | return bool: デゥエルの勝者(ボールホルダー = ture, ディフェンダー = false) |
	bool AirBattle(AC_Piece* dueledPlayer);
	// プレス
	void Press();
	// マーキング
	void Marking(AC_Piece* defencePlayer, AC_Piece* offensePlayer);
	// クリアリング
	// | defencePlayer: 対人をされるプレイヤー |
	void Clearing(AC_Piece* defencePlayer);
	// ロングクリアリング
	//  →同じColumnのディフェンスライン前にボールを送り込む
	void LongClearing();
	// キープ中、ボールホルダーへ味方が近づく
	// | ボールホルダーより後ろの味方で最も近い一人のみ |
	void ApproachBallHolderWhenBallKeeping();
	// ディフェンスラインコントロール
	// →目標ディフェンスラインへ移動
	void DefenseLineControl(AC_Piece* defensePlayer, int targetRow);
	// セカンドボールに近づく
	void AproachToSecondBall(AC_Piece* secondBallPlayer);
	// ターゲットマンのオフザボール
	//  →ボールホルダーのレーンに移動する
	void MoveToBallHolderLaneForTargetMan(AC_Piece* targetManPlayer);
	// ロングパスターゲットに近づく
	void AproachToLongPassTargetForDefender(AC_Piece* defensePlayer);
	// **************************
	


	// ********* フェーズ *********
	// 2次配置フェーズ
	void SecondPlacePhase();
	// ③-⑵プレイステップフェーズを監視するタイマー設定
	void SetTimerMonitorPlayStepPhase();
	// ③-⑵プレイステップフェーズを終了していいか監視
	void MonitorFinishPlayStepPhase();
	// (フェーズ)タイマーとステップを終了
	void FinishTimerAndStep();
	// 試合開始時処理
	void StartOfMatch();
	// ③-⑴ 準備ステップフェーズ
	void PrepareStepPhase();
	// ③-⑵ プレイステップフェーズ
	void PlayStepPhase();
	// ③-⑶ リセットステップフェーズ (bool: falseでフェーズ終了)
	bool ResetStepPhase();
	// **************************
	


	// マウス位置に目的のオブジェクトがあるか判定して情報取得
	// | retrun: 目的のオブジェクトか判定, hitRusult: 取得するオブジェクト情報, objectTypes: 目的のオブジェクトの種類(コリジョン) |
	bool GetResultFromMouseLocation(FHitResult& hitResult, TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes);
	// 位置からタイルＮｏ取得
	int GetTileNoFromLocation(float x, float y);
	// パスレンジのタイルＮｏ取得
	TArray <int> GetTileNoInPassRange();
	// ボールホルダーの向きを取得
	int GetDirectionOfBallHolder();
	// 対人範囲のタイルNo取得
	TArray <int> GetTileNoInDuelRange();
	// マークレンジ取得
	// | currentTileNo: マークレンジ元のタイルＮｏ, playerDirection: 体の向き |
	TArray <int> GetMarkRange(int currentTileNo, int playerDirection);
	// ファーストディフェンダー取得
	AC_Piece* GetFirstDefender();
	// 次に動く最短距離のタイルNo取得
	int GetShortestNextTileNo(int fromTileNo, int toTileNo);
	// プレイヤー配置可能エリアを表示
	void DisplayPlayerPlaceableErea();
	// プレイヤー配置レンジを削除
	void RemovePlayerPlaceRange();
	// 全てのプレイヤーにポジションを設定
	void SetPositionToAllPlayer();
	// 全てのプレイヤーとボールにタイル位置を設定
	TArray<int> SetTileNoToAllPlayersAndBall();
	// プレイヤーとボール位置をタイル位置へ設定
	void SetPlayerAndBallToTilePosition();
	// プレイヤー間のラインを表示
	void DisplayLineBetweenPlayers();
	// プレイヤーのレーンを設定
	void SetLaneForPlayers();
	// セーフティーパス判定
	bool CheckPassSafety(AC_Piece* player);

	
	AC_Piece* selectedPlayer = nullptr; // 選択されたプレイヤー
	AC_Ball* selectedBall = nullptr; // 選択されたボール(デバッグ時)
	bool isGrap = false; // Grap(プレイヤー選択中)フラグ
	TArray <AC_Tile*> allTiles; // すべてのタイル配列
	AC_Tile* overlayTile; // 光っているタイル
	UMaterial* playerSelectedDecal = nullptr; // プレイヤー選択デカール
	UDecalComponent* currentDecal = nullptr; // 現在表示しているデカール
	AC_Ball* ball; // ボール
	TArray <AC_Piece*> allPieces; // すべてのコマ配列
	TArray <AC_Piece*> allHomePieces; // すべてのHomeコマ配列
	TArray <AC_Piece*> allAwayPieces; // すべてのAwayコマ配列
	AC_Piece* ballHolder = nullptr; // ボールホルダー
	AC_Piece* preBallHolder = nullptr; // 前回のボールホルダー
	AC_Piece* passTarget = nullptr; // パスターゲット
	AC_Piece* longPassTarget = nullptr; // ロングパスターゲット
	bool isInitialPlacePhase = true; // 初期配置フェーズフラグ
	bool isSecondPlacePhase = false; // 2次配置フェーズフラグ
	bool isPlayStepPhase = false; // プレイステップフェーズ中
	int stepCount = 0; // ステップカウント
	TArray <int> passRangeTileNos; // パスレンジタイルNo配列
	TArray <int> duelRangeTileNos; // 対人レンジタイルNo配列
	TArray< AC_Piece*> offencePlayers; // オフェンス側のプレイヤー配列
	TArray< AC_Piece*> defencePlayers; // ディフェンス側のプレイヤー配列
	bool isHomeBall = false; // Home側がオフェンスか
	AC_Piece* firstDefender = nullptr; // ファーストディフェンダー
	TArray <int> currentTileNos = {}; // 現在のプレイヤーのタイルNo
	TArray <int> moveToTileNos = {}; // プレイヤーとボールが動く先のタイルNo (*動く先のタイルを予約し、重複を防ぐ)
	bool isGoal = false; // ゴール判定
	bool isClering = false; // クリアリング判定
	int ballHolderRightTileNo = 0; // ボールホルダーの右タイルNo
	int ballHolderLeftTileNo = 0; // ボールホルダーの左タイルNo
	int secondBallRangeTileNum = 0; // セカンドボール反応範囲のタイル個数 (step毎に拡大)
	TArray <int> secondBallRangeTileNos = {}; // セカンドボールレンジNo
	TArray <int> placeableRrea = {}; // プレイヤー配置可能エリア
	TArray <int> ballHolderAroundTileNos = {}; // ボールホルダー周囲タイルNo (エアバトル発火タイル)
	bool isAirBattle = false; // エアバトル判定
	bool isBallKeeping = false; // ボールキープ中か
	bool isPreActionedLongBall = false; // ロングボール事前動作後か
	bool isLongBalled = false; // ロングボール後か
	bool isFloatingBall = false; // 現在ボールが浮いているか (*ロングボール時ON)
	AC_Piece* nearestLongBallTargetDF = nullptr; // ロングパスターゲットに最も近いDFプレイヤー(敵)
	
	
	
	// **** オフザボール ****
	// < ターゲットマン >
	TArray <AC_Piece*> targetManPlayeres = {}; // ターゲットマンプレイヤー(全て) *試合開始時に全て取得
	TArray <AC_Piece*> myTargetMans = {}; //  対象のターゲットマン
	// ********************
	

	// **** マッチフェーズ ****
	int matchPhase = 0; // マッチフェーズNo (*プレイSTEPでは変更しない)
	bool isFinishMatchPhase = false; // マッチフェーズ終了フラグ
	int stepCountForGamePhase = 0; // ステップカウンター
	AC_Piece* nextBallHolder = nullptr; // 次のボールホルダー (*プレイSTEPで変更)
	// < セカンドボール回収 >
	TArray<int> secondBallCollectRange = {}; // セカンドボール回収範囲
	TArray<int> offenseSecondBallCollectPoints = {}; // セカンドボール回収ポイント(オフェンス側)
	TArray<int> defenseSecondBallCollectPoints = {}; // セカンドボール回収ポイント(ディフェンス側)
	AC_Piece* secondBallCollectPlayer = nullptr; // ボール回収プレイヤー
	// < クロス >
	TArray<int> handleCrossRange = {}; // クロス対応範囲
	AC_Piece* crossTargetPlayer = nullptr; // クロスのターゲット
	bool isCrossTargetOffense = false; // クロスターゲットがオフェンスか
	const TArray<int> HOME_DEFENSE_HANDLE_CROSS_POINTS = { 60, 62, 64, 66 }; // クロス対応ポイント (HOME)
	const TArray<int> AWAY_DEFENSE_HANDLE_CROSS_POINTS = { 935, 937, 939, 941 }; // クロス対応ポイント (AWAY)
	// ********************



	// **** リレーショナルプレーパターン ****
	int playPattern = 0; // プレーパターンNo (*プレイSTEPでは変更しない)
	bool isFinishPlayPattern = false; // プレーパターン終了フラグ
	int stepCountForPlayPattern = 0; // ステップカウンター
	// < ドリフトワイドプレイパターン >
	TArray <AC_Piece*> sideBreakPlayeres = {}; // サイドブレイクプレイヤー(全て) *試合開始時に全て取得
	AC_Piece* mySideBreaker = nullptr; //  対象のサイドブレイカー
	AC_Piece* follower = nullptr; //  フォロワー *サイドブレイカーと同じレーン (*SBを想定)
	// **********************************



	struct FDistanceToBallHolder // ボールホルダーとプレイヤーの距離(*比較のため)
	{
		AC_Piece* player;
		float distance;
	};
	
	// ** ポジションエリア (タイルNo) **
	// < HOME >
	// -FW-
	TArray <int> LWG_Erea = {}; // LWG
	TArray <int> LST_Erea = {}; // LST
	TArray <int> CF_Erea = {}; // CF
	TArray <int> RST_Erea = {}; // RST
	TArray <int> RWG_Erea = {}; // RWG
	// -MF-
	TArray <int> LH_Erea = {}; // LH
	TArray <int> LIH_Erea = {}; // LIH
	TArray <int> CH_Erea = {}; // CH
	TArray <int> RIH_Erea = {}; // RIH
	TArray <int> RH_Erea = {}; // RH
	// -DF-
	TArray <int> LSB_Erea = {}; // LSB
	TArray <int> LCB_Erea = {}; // LCB
	TArray <int> CB_Erea = {}; // CB
	TArray <int> RCB_Erea = {}; // RCB
	TArray <int> RSB_Erea = {}; // RSB
	
	// < AWAY >
	// -FW-
	TArray <int> Away_LWG_Erea = {}; // LWG
	TArray <int> Away_LST_Erea = {}; // LST
	TArray <int> Away_CF_Erea = {}; // CF
	TArray <int> Away_RST_Erea = {}; // RST
	TArray <int> Away_RWG_Erea = {}; // RWG
	// -MF-
	TArray <int> Away_LH_Erea = {}; // LH
	TArray <int> Away_LIH_Erea = {}; // LIH
	TArray <int> Away_CH_Erea = {}; // CH
	TArray <int> Away_RIH_Erea = {}; // RIH
	TArray <int> Away_RH_Erea = {}; // RH
	// -DF-
	TArray <int> Away_LSB_Erea = {}; // LSB
	TArray <int> Away_LCB_Erea = {}; // LCB
	TArray <int> Away_CB_Erea = {}; // CB
	TArray <int> Away_RCB_Erea = {}; // RCB
	TArray <int> Away_RSB_Erea = {}; // RSB
	// ***

	// ポジション範囲
	const TArray <int> DFPositionRange = { 1, 2, 3, 4, 5 }; // DF (*GKは含まない)
	const TArray <int> MFPositionRange = { 6, 7, 8, 9, 10 }; // MF
	const TArray <int> FWPositionRange = { 11, 12, 13, 14, 15 }; // FW
	// 現在のディフェンスラインRow
	// | 0 - 39まで |
	int home_currentDefenseLineRow = 0;
	int away_currentDefenseLineRow = 0;
	// 目標のディフェンスラインRow
	int home_targetDefenseLineRow = 0;
	int away_targetDefenseLineRow = 0;
	// ペナルティーエリア範囲
	const TArray <int> HOME_PENALTY_EREA_RANGE = { // HOME
		6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
		31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
		56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
		81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
		106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
		131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145
	};
	const TArray <int> AWAY_PENALTY_EREA_RANGE = { // AWAY
		856, 857, 858, 859, 860, 861, 862, 863, 864, 865, 866, 867, 868, 869, 870,
		881, 882, 883, 884, 885, 886, 887, 888, 889, 890, 891, 892, 893, 894, 895,
		906, 907, 908, 909, 910, 911, 912, 913, 914, 915, 916, 917, 918, 919, 920,
		931, 932, 933, 934, 935, 936, 937, 938, 939, 940, 941, 942, 943, 944, 945,
		956, 957, 958, 959, 960, 961, 962, 963, 964, 965, 966, 967, 968, 969, 970,
		981, 982, 983, 984, 985, 986, 987, 988, 989, 990, 991, 992, 993, 994, 995
	};
	// tags
	const FName TARGET_MAN_TAG = "TARGET_MAN";
	const FName SIDE_BREAKER_TAG = "SIDE_BREAKER";
	const FName HOME_PLAYER_TAG = "HOME";
	const FName AWAY_PLAYER_TAG = "AWAY";
	const FName BALL_TAG = "BALL";
};
