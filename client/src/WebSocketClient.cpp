#include "WebSocketClient.h"

// サーバーアドレスやポートなどを初期化
WebSocketClient::WebSocketClient(const char *serverAddr, uint16_t serverPort)
    : server(serverAddr), port(serverPort), connected(false), websocketKey("dGhlIHNhbXBsZSBub25jZQ==") {}

// デストラクタで通信を切断
WebSocketClient::~WebSocketClient()
{
    disconnect();
}

// サーバーに接続する
bool WebSocketClient::connectToServer()
{
    Serial.println("サーバーへの接続を試みます");
    // ipアドレスを元に指定のポートで接続
    if (client.connect(server.c_str(), port))
    {
        Serial.println("サーバーへの接続しました");

        // WebSocketハンドシェイクリクエスト送信
        client.println("GET / HTTP/1.1");
        client.print("Host: ");
        client.print(server);
        client.print(":");
        client.println(port);
        client.println("Upgrade: websocket");
        client.println("Connection: Upgrade");
        client.print("Sec-WebSocket-Key: ");
        client.println(websocketKey);
        client.println("Sec-WebSocket-Version: 13");
        client.println();

        // ハンドシェイクレスポンスを待機
        delay(2000);

        // レスポンスを読み込み
        while (client.available())
        {
            // 改行が来るまで一文字ずつ読み取る
            String line = client.readStringUntil('\n');
            // シリアルモニターに出力
            Serial.println(line);
            if (line == "\r")
            {
                break;
            }
        }

        Serial.println("サーバーへの接続が確立されました");
        connected = true;
        return true;
    }
    else
    {
        Serial.println("サーバーへの接続に失敗しました");
        connected = false;
        return false;
    }
}

// 通信を切断
void WebSocketClient::disconnect()
{
    if (client.connected())
        client.stop();
    connected = false;
}

// 接続されているかチェック
bool WebSocketClient::isConnected()
{
    return connected && client.connected();
}

void WebSocketClient::sendData(uint8_t x, uint8_t y, uint8_t life)
{
    // バイナリペイロード（3バイト）
    byte payload[3];

    // 追加 プレイヤー1の座標と残機を代入--------------------------------------
    payload[0] = x;
    payload[1] = y;
    payload[2] = life;

    int payloadLength = 3;

    // WebSocketバイナリフレーム（opcode = 0x82）
    client.write(0x82);                 // FIN=1, opcode=0x2 (binary frame)
    client.write(0x80 | payloadLength); // MASK=1, payload length (3バイト)

    // マスクキー（4バイト）
    byte maskKey[4] = {0x12, 0x34, 0x56, 0x78};
    client.write(maskKey, 4);

    // ペイロードをマスクして送信
    for (int i = 0; i < payloadLength; i++)
    {
        client.write(payload[i] ^ maskKey[i % 4]);
    }

    Serial.print("バイナリメッセージを送信: x=");
    Serial.print(payload[0]);
    Serial.print(", y=");
    Serial.print(payload[1]);
    Serial.print(", life=");
    Serial.println(payload[2]);
}

bool WebSocketClient::receiveData(uint8_t &x, uint8_t &y, uint8_t &life)
{
    byte firstByte = 0;
    byte secondByte = 0;
    byte opcode = 0;
    int payloadLength = 0;

    if (client.available())
    {
        // WebSocketフレームを読み込み
        firstByte = client.read();
        secondByte = client.read();

        // フレームタイプを確認
        opcode = firstByte & 0x0F;

        if (opcode == 0x02)
        { // バイナリフレーム
            payloadLength = secondByte & 0x7F;

            if (payloadLength >= 3 && client.available() >= 3)
            {
                x = client.read();
                y = client.read();
                life = client.read();

                Serial.print("バイナリメッセージを受信: x=");
                Serial.print(x);
                Serial.print(", y=");
                Serial.print(y);
                Serial.print(", life=");
                Serial.println(life);
            }

            // 残りのバイトを読み飛ばし
            while (payloadLength > 3 && client.available())
            {
                client.read();
                payloadLength--;
            }
        }
        else
        {
            // その他のフレームタイプ（テキスト、ping/pong等）
            Serial.print("未対応のフレームタイプ: 0x");
            Serial.println(opcode, HEX);

            payloadLength = secondByte & 0x7F;
            for (int i = 0; i < payloadLength && client.available(); i++)
            {
                client.read();
            }
        }
        return true;
    }
    return false;
}