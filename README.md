Event Driven Architecture Flow Chart

```
[インプット]  
・C_My_Player_Controller (player controller)  
      ▼  
[監視・キャスト（イベント発行）]
・GameStateManager (subsystem)
      ▼  
[購読（リスナー）]
・CPlayerAI (AI controller)
      ▼  
[アクション（副作用）]
・C_Player (Character)
・C_Manager (Character)
・C_Soccer_Ball (Pawn)
・C_Opening_UI (UserWidget)
・C_Tile (Actor)
```
