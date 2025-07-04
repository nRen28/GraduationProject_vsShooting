#pragma once
#include <wifiS3.h>
#include "types.h"

class WebSocketClient {
private:
    WiFiClient client;
    String server;
    uint16_t port;
    bool connected;

    String websocketKey;

public:
    WebSocketClient(const char* serverAddr, uint16_t serverPort);
    ~WebSocketClient();
    
    //サーバーに接続する
    bool connectToServer();
    //通信を切断
    void disconnect();
    //接続されているかチェック
    bool isConnected();

     //自身の座標とライフを送信する
    void sendData(uint8_t x,uint8_t y,uint8_t life);
    //通信相手の座標とライフを受信する
    bool receiveData(uint8_t &x,uint8_t &y,uint8_t &life);
    
private:
   
};