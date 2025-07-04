#include "types.h"
#include "sceneManager.h" //シーンマネージャー
#include "scene_title.h"  //タイトル画面 最初に呼び出すシーン
#include "EEPROM.h"

#include "WebSocketClient.h" // 新しく作成したWebSocketクラス
#include "FspTimer.h"        //割り込み処理で必要なライブラリ

#include "arduino_secrets.h"

#define REFRESHRATE 1 // 通信の間隔

int keyIndex = 0;

volatile int status = WL_IDLE_STATUS;

// WebSocketクライアントのインスタンス
WebSocketClient *webSocketClient = nullptr;

// シーンマネージャーのインスタンス
SceneManager *sceneManager = nullptr;

// 割り込みで使用する変数等-----------------------------------
FspTimer _timer;
volatile bool shouldDraw = false;
volatile bool gameState = GAME_RUNNING;

const char *server = "192.168.0.14"; // サーバーのIPアドレス
const int port = 8080;               // WebSocketポート

WebSocketClient *wsClient = new WebSocketClient(server, port);

// 初期化--------------------------------------------------------------------------------------------------------------------------------------
void setup()
{

  Serial.begin(115200);
  while (!Serial)
  {
    ; // シリアルポートの接続を待機（ネイティブUSBポートのみ必要）
  }

  // ジョイスティックの軸とスイッチ
  pinMode(STICK_X, INPUT);
  pinMode(STICK_Y, INPUT);
  pinMode(BUTTON_A, INPUT);

  // 起動時に12番ピンがLOWならハイスコアを消去する
  pinMode(12, INPUT_PULLUP);
  if (digitalRead(12) == LOW)
  {
    EEPROM.write(EEPROM_ADDR_HISCORE,0);
  }

  delay(5000);

  // if (!wsClient->isConnected())
  //   {
  //       /// WiFiモジュールのチェック:
  //       if (WiFi.status() == WL_NO_MODULE)
  //       {
  //           Serial.println("WiFiモジュールとの通信に失敗しました!");
  //           // 処理を続行せず停止
  //           while (true)
  //               ;
  //       }

  //       String fv = WiFi.firmwareVersion();
  //       if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  //       {
  //           Serial.println("ファームウェアをアップグレードしてください");
  //       }

  //       // WiFiネットワークへの接続試行:
  //       while (status != WL_CONNECTED)
  //       {
  //           Serial.print("SSIDへ接続を試みています: ");
  //           Serial.println(SECRET_SSID);
  //           // WPA/WPA2ネットワークへの接続。オープンネットワークやWEPを使用する場合は変更:
  //           status = WiFi.begin(SECRET_SSID, SECRET_PASS);

  //           // 接続待機（3秒間）:
  //           delay(3000);
  //       }

  //       // WebSocketサーバーへの接続
  //       while (1)
  //       {
  //           if (wsClient->connectToServer())
  //           {
  //               break;
  //           }
  //       }
  //   }

  // wsClient->connectToServer();

  // OLEDを初期化
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // ディスプレイをクリア
  display.clearDisplay();

  // // ゲーム部分-------------------------------------------------------------------
  // シーンマネージャーをnewする
  sceneManager = new SceneManager;
  // 最初のシーン
  sceneManager->currentScene = new Title(sceneManager);
  // タイマー割り込み------------------------------------------------------
  uint8_t type;
  int8_t ch = FspTimer::get_available_timer(type);
  if (ch < 0)
  {
    return;
  }
  _timer.begin(TIMER_MODE_PERIODIC, type, ch, 60.0f, 50.0f, callbackfunc, nullptr);
  _timer.setup_overflow_irq(15, nullptr);
  _timer.open();
  _timer.start();
}

// メインループ--------------------------------------------------------------------------------------------------------------------------
void loop()
{
  // 描画フラグが立っていたらゲームの描画を行う
  if (shouldDraw)
  {
    display.clearDisplay(); // ディスプレイをクリア
    display.setTextColor(WHITE);
    sceneManager->drawScene(); // シーンの描画
    display.display();         // 画面を更新
    shouldDraw = false;
  }

  // 通信処理（通信しないシーンなら何も起きない）
  sceneManager->communicateScene();
}

// タイマー割り込みで呼ばれる関数--------------------------------------------------------------------------------------------
void callbackfunc(timer_callback_args_t *arg)
{
  if (gameState == GAME_RUNNING)
  {
    gameState = sceneManager->updateScene(); // ゲームシーンを更新
  }
  shouldDraw = true;
}