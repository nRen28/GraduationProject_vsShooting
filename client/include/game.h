#pragma once
#include "types.h"
#include "objManager.h" //オブジェクト管理クラス

// 関数-------------------------------------------------------
// // オブジェクトマネージャー
// extern ObjManager objManager;

// ゲームの初期化
void initializeGame();
// シングルモードの初期化
void initializeSingleMode();
// 対戦モードの初期化
void initializeVersusMode();

// ゲームの更新処理
int runGame();
// シングルモードの処理
int runSingleMode();
// 対戦モードの処理
int runVersusMode();

// 描画処理
void drawGraph();
void drawSingleMode();
void drawVersusMode();

// スタート前のカウントダウン
void startCountdown();

// 変数--------------------------------------------------------
// ゲームのモード
extern bool vsmode;