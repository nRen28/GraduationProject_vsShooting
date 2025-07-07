#include "../config/arduino_secrets.h"
#include "scene_versusMode.h"
#include "sceneManager.h"
#include "scene_title.h"
#include "player.h"

#define COUNTDOWN_DURATION 60

VersusMode::VersusMode(SceneManager *p) : SceneBase(p)
{
    // アクセスポイントの設定
    ssid = SECRET_SSID;
    pass = SECRET_PASS;
    // サーバーの設定
    server = WEBSOCKET_IP;
    port = WEBSOCKET_PORT;

    // WebSocketClientを動的に作成
    wsClient = new WebSocketClient(server, port);
    status = WL_IDLE_STATUS;

    // 通信制御フラグを初期化
    m_shouldCommunicate = true;
    m_isMatched = false;

    // ゲームの状態を初期化
    m_countDown = 3;
    m_timer = 0;
    m_gameState = STATE_WAITING_FOR_OPPONENT;
    m_p1win = false;
    m_p2win = false;

    // プレイヤー生成
    objManager.addObj(new Player({0, SCREEN_HEIGHT / 2 - 5}, PLAYER1, &objManager));                 // player1
    objManager.addObj(new Player({SCREEN_WIDTH - 10, SCREEN_HEIGHT / 2 - 5}, PLAYER2, &objManager)); // player2
}

int VersusMode::update()
{
    // ゲームの状態
    switch (m_gameState)
    {
    // 対戦相手待機中
    case STATE_WAITING_FOR_OPPONENT:
        // 通信に成功したらカウントダウンへ
        if (m_isMatched)
        {
            m_gameState = STATE_COUNTDOWN;
        }
        break;
    // 対戦開始前のカウントダウン
    case STATE_COUNTDOWN:
        // 60フレーム毎にカウントを減らす
        if (m_timer == COUNTDOWN_DURATION)
        {
            // カウントダウンが終わったらゲーム開始
            if (m_countDown == 0)
            {
                m_gameState = STATE_IN_GAME;
            }
            else
            {
                m_countDown--;
            }
            m_timer = 0;
        }
        m_timer++;
        // カウントダウンが終わったらゲームへ

        break;
    // ゲーム中
    case STATE_IN_GAME:
        // どちらかが死ぬまでゲームを更新
        if (!m_p1win && !m_p2win)
        {
            // オブジェクト更新
            objManager.updateObj();

            // オブジェクトクリーンアップ
            objManager.cleanupObj();

            // 勝敗チェック用
            m_p1win = true;
            m_p2win = true;
            // 画面上に表示する残機更新と勝敗チェック
            for (int i = 0; i < MAX_OBJ; i++)
            {
                if (objManager.getObjPtr(i) != nullptr && objManager.getObjPtr(i)->m_id == PLAYER1)
                {
                    m_p1Life = ((Player *)objManager.getObjPtr(i))->get_life();
                    m_p2win = false;
                }

                if (objManager.getObjPtr(i) != nullptr && objManager.getObjPtr(i)->m_id == PLAYER2)
                {
                    m_p2Life = ((Player *)objManager.getObjPtr(i))->get_life();
                    m_p1win = false;
                }
            }
        }
        else
        {
            // 結果画面へ
            m_gameState = STATE_RESULT;
            wsClient->disconnect();
        }
        break;
    // 結果画面
    case STATE_RESULT:
        // タイトルに戻る
        if (digitalRead(BUTTON_A) == HIGH)
        {
            sceneManager->deleteScene();
            sceneManager->currentScene = new Title(sceneManager);
        }
        break;
    }
    return GAME_RUNNING;
}

void VersusMode::draw()
{
    // ゲームの状態に応じて描画
    switch (m_gameState)
    {
    // 対戦相手待機中
    case STATE_WAITING_FOR_OPPONENT:
        display.setCursor(0, 30);
        display.print("Please wait for\n your opponent.");
        break;
    // 対戦開始前のカウントダウン
    case STATE_COUNTDOWN:
        if (m_countDown > 0)
        {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(45, 30);
            display.print(m_countDown);
        }
        else if (m_countDown == 0)
        {
            display.setTextSize(2);
            display.setTextColor(WHITE);
            display.setCursor(15, 30);
            display.print("START!!");
        }
        break;
    // ゲーム中
    case STATE_IN_GAME:
        // ゲーム中央に引く境界線
        for (int y = 0; y < SCREEN_HEIGHT; y += 2)
        {
            display.drawPixel(SCREEN_WIDTH / 2, y, WHITE); // 縦に1ピクセルずつ線を描画
        }
        // オブジェクト描画
        objManager.drawObj();

        display.setTextSize(1);                       // 文字サイズ（1が基本サイズ）
        display.setCursor(0, 0);                      //(x, y) = (0, 0) は1行目の左端
        display.print("P1 LIFE:" + String(m_p1Life)); // 残機を表示
        display.setCursor(70, 0);
        display.print("P2 LIFE:" + String(m_p2Life));
        break;
    // 結果画面
    case STATE_RESULT:
        display.setTextSize(2);
        display.setCursor(0, 20);
        if (m_p1win && m_p2win) // 念のため引き分けも調べる
        {
            display.print("DRAW...");
        }
        else if (m_p1win) // 自分の勝ち
        {
            display.print("WIN!!");
        }
        else if (m_p2win) // 自分の負け
        {
            display.print("LOSE...");
        }
        display.setTextSize(1);
        display.setCursor(0, 45);
        display.print("Press a button to\n return");
        break;
    }
}

// 通信処理
void VersusMode::communicate()
{
    // 通信が無効化されている場合は何もしない
    if (!m_shouldCommunicate)
    {

        return;
    }

    switch (m_gameState)
    {
    // 対戦相手待機中
    case STATE_WAITING_FOR_OPPONENT:
    {
        int retryCount = 0;
        const int MAX_RETRIES = 3;

        if (!wsClient->isConnected())
        {
            /// WiFiモジュールのチェック:
            if (WiFi.status() == WL_NO_MODULE)
            {
                Serial.println("WiFiモジュールとの通信に失敗しました!");
                // 処理を続行せず停止
                while (true)
                    ;
            }

            String fv = WiFi.firmwareVersion();
            if (fv < WIFI_FIRMWARE_LATEST_VERSION)
            {
                Serial.println("ファームウェアをアップグレードしてください");
            }

            // WiFiネットワークへの接続試行:
            while (1)
            {
                display.setCursor(0, 20);
                display.clearDisplay(); // ディスプレイをクリア
                display.print("Connecting to\n accesspoint...");
                display.display();

                Serial.print("SSIDへ接続を試みています: ");
                Serial.println(ssid);
                // WPA/WPA2ネットワークへの接続。オープンネットワークやWEPを使用する場合は変更:
                status = WiFi.begin(ssid, pass);

                // 接続待機（3秒間）:
                delay(3000);

                // WiFiにつながったらループから抜ける
                if (status == WL_CONNECTED)
                {
                    retryCount = 0;
                    break;
                }
                // 一定回数失敗したら中止
                retryCount++;
                if (retryCount >= MAX_RETRIES)
                {
                    connectionError();
                }
            }
            // WebSocketサーバーへの接続
            while (1)
            {
                display.setCursor(0, 20);
                display.clearDisplay(); // ディスプレイをクリア
                display.print("Connecting to\n server...");
                display.display();
                // サーバーにつながったらループから抜ける
                if (wsClient->connectToServer())
                {
                    retryCount = 0;
                    break;
                }
                // 一定回数失敗したら中止
                retryCount++;
                if (retryCount >= MAX_RETRIES)
                {
                    connectionError();
                }
            }
        }
        else // 対戦相手のライフが255以外になったら対戦相手が参加したという判定
        {
            // データの送受信
            receivePlayer2Data();
            sendPlayer1Data();
        }
    }
    break;
    // 対戦開始前のカウントダウン
    case STATE_COUNTDOWN:
        // データの送受信
        receivePlayer2Data();
        sendPlayer1Data();
        break;
    // ゲーム中
    case STATE_IN_GAME:
        // データの送受信
        receivePlayer2Data();
        sendPlayer1Data();
        break;
    // 結果画面
    case STATE_RESULT:
        break;
    }
}

// 通信が上手くいかなかった場合の処理
void VersusMode::connectionError()
{
    display.clearDisplay();
    display.setCursor(0, 20);
    display.print("Connection error!");
    display.setCursor(0, 35);
    display.print("Please press the\n reset button.");
    display.display();

    while (1)
    {
        delay(10);
    }
}

// プレイヤー1のデータを送信する関数
void VersusMode::sendPlayer1Data()
{
    for (int i = 0; i < MAX_OBJ; i++)
    {
        ObjBase *ptr_obj = objManager.getObjPtr(i);
        if (ptr_obj != nullptr && ptr_obj->m_id == PLAYER1)
        {
            Serial.println("送信します");
            wsClient->sendData((uint8_t)ptr_obj->m_pos.x, (uint8_t)ptr_obj->m_pos.y, ((Player *)ptr_obj)->get_life());
            break;
        }
    }
}

// プレイヤー2のデータを受信して反映する関数
void VersusMode::receivePlayer2Data()
{
    for (int i = 0; i < MAX_OBJ; i++)
    {
        ObjBase *ptr_obj = objManager.getObjPtr(i);
        if (ptr_obj != nullptr && ptr_obj->m_id == PLAYER2)
        {
            uint8_t x, y, life;
            bool received = wsClient->receiveData(x, y, life);

            if (received)
            {
                Serial.println("受信しました");
                // マッチング状態をチェック
                // life値が255の場合は1人待機状態、それ以外はマッチング成立
                if (life == 255)
                {
                    m_isMatched = false; // まだマッチングしていない
                }
                else
                {
                    m_isMatched = true; // マッチング成立

                    // プレイヤー2の座標を更新（左右反転）
                    ptr_obj->m_pos.x = SCREEN_WIDTH - x - ptr_obj->m_width;
                    ptr_obj->m_pos.y = y;
                    // 残機を更新
                    ((Player *)ptr_obj)->set_life(life);
                }
            }
            break;
        }
    }
}

VersusMode::~VersusMode()
{
    delete wsClient;
    wsClient = nullptr;
}