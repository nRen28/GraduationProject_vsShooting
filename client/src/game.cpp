// #include "game.h"
// // ゲームオブジェクト
// #include "player.h"
// #include "enemy.h"
// #include "bullet.h"

// // オブジェクトマネージャー
// ObjManager objManager;

// // 画面に表示するプレイヤーの残機数
// uint8_t p1Life = 2;
// uint8_t p2Life = 2;
// // シングルプレイヤーモードで表示するスコア
// uint8_t score = 0;

// // 勝敗フラグ用
// bool p1win = false;
// bool p2win = false;

// // ゲームの初期化
// void initializeGame()
// {
// }

// // シングルプレイヤーモードの初期化(後々カウントダウンの場所を変更する予定)
// void initializeVersusMode()
// {

//   startCountdown();

//   // プレイヤー生成
//   objManager.addObj(new Player({0, SCREEN_HEIGHT / 2 - 5}, PLAYER1, &objManager));                 // player1
//   objManager.addObj(new Player({SCREEN_WIDTH - 10, SCREEN_HEIGHT / 2 - 5}, PLAYER2, &objManager)); // player2
// }

// void initializeSingleMode()
// {
//   startCountdown();

//   // プレイヤーとエネミー生成
//   objManager.addObj(new Player({0, SCREEN_HEIGHT / 2 - 5}, PLAYER1, &objManager));
//   objManager.addObj(new Enemy(&objManager));
// }

// // ゲームの更新処理　タイマー割り込みなのでdelayやシリアルモニター等は使えない
// int runGame()
// {
//   // オブジェクト更新
//   objManager.updateObj();

//   // オブジェクトクリーンアップ
//   objManager.cleanupObj();

//   // ゲームの種類によって処理が変わる部分
//   if (vsmode)
//   {
//     return runVersusMode();
//   }
//   else
//   {
//     return runSingleMode();
//   }
// }

// // シングルモードの処理
// int runSingleMode()
// {
//   // プレイヤーが死んだか判定用
//   p2win = true;
//   // 画面上に表示する残機とスコア更新
//   for (int i = 0; i < MAX_OBJ; i++)
//   {
//     if (objManager.getObjPtr(i) != nullptr && objManager.getObjPtr(i)->m_id == PLAYER1)
//     {
//       p1Life = ((Player *)objManager.getObjPtr(i))->get_life();
//       p2win = false;
//     }

//     if (objManager.getObjPtr(i) != nullptr && objManager.getObjPtr(i)->m_id == ENEMY)
//     {
//       score = ((Enemy *)objManager.getObjPtr(i))->get_hitCount();
//     }
//   }

//   // プレイヤーが死んだら終了
//   if (p2win)
//   {
//     return GAME_FINISHED;
//   }

//   return GAME_RUNNING;
// }

// // 対戦モードの処理
// int runVersusMode()
// {
//   // 勝敗チェック用
//   p1win = true;
//   p2win = true;
//   // 画面上に表示する残機更新と勝敗チェック
//   for (int i = 0; i < MAX_OBJ; i++)
//   {
//     if (objManager.getObjPtr(i) != nullptr && objManager.getObjPtr(i)->m_id == PLAYER1)
//     {
//       p1Life = ((Player *)objManager.getObjPtr(i))->get_life();
//       p2win = false;
//     }

//     if (objManager.getObjPtr(i) != nullptr && objManager.getObjPtr(i)->m_id == PLAYER2)
//     {
//       p2Life = ((Player *)objManager.getObjPtr(i))->get_life();
//       p1win = false;
//     }
//   }

//   // どちらかがwinの場合、ゲーム終了
//   if (p1win || p2win)
//   {
//     return GAME_FINISHED;
//   }

//   return GAME_RUNNING;
// }

// // 描画処理
// void drawGraph()
// {
//   // ディスプレイをクリア
//   display.clearDisplay();

//   // ゲーム中央に引く境界線
//   for (int y = 0; y < SCREEN_HEIGHT; y += 2)
//   {
//     display.drawPixel(SCREEN_WIDTH / 2, y, WHITE); // 縦に1ピクセルずつ線を描画
//   }

//   // オブジェクト描画
//   objManager.drawObj();

//   // ゲームの種類によって描画内容が変わる
//   if (vsmode)
//   {
//     drawVersusMode();
//   }
//   else
//   {
//     drawSingleMode();
//   }

// }

// void drawSingleMode()
// {
//   // UI---------------------------------------------------------------------------------
//   display.setTextSize(1);                     // 文字サイズ（1が基本サイズ）
//   display.setTextColor(WHITE);                // 文字色
//   display.setCursor(0, 0);                    //(x, y) = (0, 0) は1行目の左端
//   display.print("P1 LIFE:" + String(p1Life)); // 残機を表示
//   display.setCursor(80, 0);
//   display.print("SCORE:" + String(score));

//   // ゲーム終了時に出すゲームオーバー画面
//   if (p2win)
//   {
//     display.clearDisplay();
//     display.setTextSize(2);
//     display.setCursor(0, 30);
//     display.print("GAME OVER!");
//   }
// }

// void drawVersusMode()
// {
//   // UI---------------------------------------------------------------------------------
//   display.setTextSize(1);                     // 文字サイズ（1が基本サイズ）
//   display.setTextColor(WHITE);                // 文字色
//   display.setCursor(0, 0);                    //(x, y) = (0, 0) は1行目の左端
//   display.print("P1 LIFE:" + String(p1Life)); // 残機を表示
//   display.setCursor(70, 0);
//   display.print("P2 LIFE:" + String(p2Life));

//   // ゲーム終了時に出す勝敗
//   if (p1win && p2win) // 念のため引き分けも調べる
//   {
//     display.clearDisplay();
//     display.setTextSize(3);
//     display.setCursor(0, 30);
//     display.print("DRAW...");
//   }
//   else if (p1win) // 自分の勝ち
//   {
//     display.clearDisplay();
//     display.setTextSize(3);
//     display.setCursor(0, 30);
//     display.print("WIN!!");
//   }
//   else if (p2win) // 自分の負け
//   {
//     display.clearDisplay();
//     display.setTextSize(3);
//     display.setCursor(0, 30);
//     display.print("LOSE...");
//   }
// }

// // スタート前のカウントダウン
// void startCountdown()
// {
//   display.setTextSize(3);
//   display.setTextColor(WHITE);
//   display.setCursor(40, 30);
//   // ゲーム開始前のカウントダウン
//   for (int i = 3; i > 0; i--)
//   {
//     display.clearDisplay();
//     display.print(i);
//     display.display();
//     delay(1000);
//   }
//   display.clearDisplay();
//   display.setTextSize(2);
//   display.setCursor(10, 30);
//   display.print("START!!");
//   display.display();
//   delay(1000);
// }