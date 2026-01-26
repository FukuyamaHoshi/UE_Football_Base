Event Driven Architecture Flow Chart

```
[インプット]  
・Systems/Controller/C_My_Player_Controller.h (player controller) 入力処理・フェーズ(ゲーム全体)変更
      |  
      ▼  
[監視・キャスト (Hub)]
・Systems/GameStateManager.h (Subsystem) フェーズ(試合の状況)変更・試合状況検知
  📢 Event Dispatcher: [ Broadcast ] を発火
      |  
      ▼  
[購読（リスナー）]
・Characters/Controller/CPlayerAI.h (AI controller) プレイヤーのプレイ判断
      |  
      ▼  
[アクション（副作用）]
・Characters/C_Player.h (Character) プレイヤー
・Characters/C_Manager.h (Character) 監督 (*現在,リスナーと兼任)
・Actors/C_Soccer_Ball.h (Pawn) ボール (*現在,リスナーと兼任)
```
