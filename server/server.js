const WebSocket = require('ws');
const PORT = 8080;
const wss = new WebSocket.Server({ port: PORT });

// クライアント管理
let client1 = null;
let client2 = null;

// データ保持用
let client1Data = { x: 96, y: 32, life: 2 };
let client2Data = { x: 96, y: 32, life: 2 };

// 定数
const WAITING_LIFE = 255;

// マッチング状態
let isMatched = false;

console.log(`WebSocketサーバー起動: ws://localhost:${PORT}`);

wss.on('connection', (ws, req) => {
    const ip = req.socket.remoteAddress;

    // クライアント割り当て
    if (!client1) {
        client1 = ws;
        console.log(`client1接続: ${ip}`);
        isMatched = false;
        sendWaitingToClient(client1);
    } else if (!client2) {
        client2 = ws;
        console.log(`client2接続: ${ip}`);
        isMatched = true;
        sendInitialData();
    } else {
        console.log(`接続拒否（満員）: ${ip}`);
        ws.close(1000, 'サーバー満員');
        return;
    }

    //送信処理
    ws.on('message', (data) => {
        if (data.length >= 3) {
            const x = data[0];
            const y = data[1];
            const life = data[2];

            const sender = ws === client1 ? 'client1' : 'client2';
            console.log(`${sender} からデータ受信: x=${x}, y=${y}, life=${life}`);

            // データを記録
            if (ws === client1) {
                client1Data = { x, y, life };
            } else if (ws === client2) {
                client2Data = { x, y, life };
            }

            // マッチングが成立していれば相手に中継
            if (isMatched) {
                const target = ws === client1 ? client2 : client1;
                if (target && target.readyState === WebSocket.OPEN) {
                    const buffer = Buffer.from([x, y, life]);
                    target.send(buffer);
                }
            } else {
                // client1に「待機中」を通知（life = 255）
                if (ws === client1) {
                    sendWaitingToClient(client1);
                }
            }
        }
    });

    // //デバッグ用送信処理
    // ws.on('message', (data) => {
    //     if (data.length >= 3) {
    //         const x = data[0];
    //         const y = data[1];
    //         const life = data[2];

    //         if (ws === client1) {
    //             client1Data = { x, y, life };

    //             if (isMatched) {
    //                 if (client2 && client2.readyState === WebSocket.OPEN) {
    //                     client2.send(Buffer.from([x, y, life]));
    //                 }
    //             } else {
    //                 //client1のデータをそのまま自分に送り返す
    //                 const debugBuffer = Buffer.from([x, y, life]);
    //                 ws.send(debugBuffer);
    //             }
    //         }
    //         else if (ws === client2) {
    //             client2Data = { x, y, life };

    //             if (client1 && client1.readyState === WebSocket.OPEN) {
    //                 client1.send(Buffer.from([x, y, life]));
    //             }
    //         }
    //     }
    // });

    ws.on('close', () => {
        if (ws === client1) {
            console.log('client1が切断されました');
            client1 = null;
            client1Data = { x: 96, y: 32, life: 2 };
        } else if (ws === client2) {
            console.log('client2が切断されました');
            client2 = null;
            client2Data = { x: 96, y: 32, life: 2 };
        }

        // マッチング状態を再判定
        isMatched = (client1 && client2);

        // 待機中の場合、client1に通知
        if (client1 && !isMatched) {
            sendWaitingToClient(client1);
        }
    });

    ws.on('error', (err) => {
        console.error(`WebSocketエラー: ${err}`);
    });
});

// client1 に life=255 の「待機中」を通知
function sendWaitingToClient(client) {
    if (client && client.readyState === WebSocket.OPEN) {
        const buffer = Buffer.from([client2Data.x, client2Data.y, WAITING_LIFE]);
        client.send(buffer);
        console.log('client1に「待機中」通知（life=255）');
    }
}

// マッチング成立時、双方に初期データを送信
function sendInitialData() {
    if (client1 && client1.readyState === WebSocket.OPEN) {
        const buffer1 = Buffer.from([client2Data.x, client2Data.y, client2Data.life]);
        client1.send(buffer1);
    }
    if (client2 && client2.readyState === WebSocket.OPEN) {
        const buffer2 = Buffer.from([client1Data.x, client1Data.y, client1Data.life]);
        client2.send(buffer2);
    }
}
