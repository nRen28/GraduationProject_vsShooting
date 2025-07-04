#pragma once
#include "sceneBase.h"

class SingleMode : public SceneBase
{
private:
    // --- ゲーム状態関連 ---
    // ゲームの状態
    enum GameState
    {
        STATE_COUNTDOWN,            // カウントダウン
        STATE_IN_GAME,              // 対戦中
        STATE_RESULT                // 結果表示
    };
    GameState m_gameState;

    // ゲーム開始前のカウントダウン用
    uint8_t m_timer = 0;
    uint8_t m_countDown = 3;


    // --- ゲームオブジェクト関連 ---
    // オブジェクトマネージャー
    ObjManager m_objManager;


    // --- プレイヤー情報・スコア・勝敗関連 ---
    // 画面に表示するプレイヤーの残機数
    uint8_t m_p1Life = 2;
    // シングルプレイヤーモードで表示するスコア
    uint8_t m_score = 0;
    uint8_t m_hiscore = 0;
    // 勝敗フラグ用
    bool m_p1death = false;


public:
    SingleMode(SceneManager *p);
    int update();
    void draw();
    ~SingleMode();
};