#pragma once
#include "sceneBase.h"
#include "WebSocketClient.h"

class VersusMode : public SceneBase
{
private:
    // --- 通信関連 ---
    char *ssid;
    char *pass;
    char* server;
    int port;

    //WebSocketClientのポインタ
    WebSocketClient *wsClient = nullptr;

    // WiFiの接続状態
    int status;

    // 対戦相手と接続状態にあるかどうか
    bool m_isMatched{false};

    // 通信制御フラグ
    bool m_shouldCommunicate{true};


    // --- ゲーム状態関連 ---
    // ゲームの状態
    enum GameState
    {
        STATE_WAITING_FOR_OPPONENT, // 待機中
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
    ObjManager objManager;


    // --- プレイヤー情報・勝敗関連 ---
    // 画面に表示するプレイヤーの残機数
    uint8_t m_p1Life = 2;
    uint8_t m_p2Life = 2;

    // 勝敗フラグ
    bool m_p1win = false;
    bool m_p2win = false;


public:
    VersusMode(SceneManager *p);
    int update();
    void draw();
    void communicate() override; // 通信処理を定義
    void connectionError();
    void sendPlayer1Data();
    void receivePlayer2Data();
    ~VersusMode();
};