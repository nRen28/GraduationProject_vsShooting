const WebSocket = require('ws');
const PORT = 18080;// 他のアプリと競合する場合や環境で固定値が必要な場合は適宜変更してください。
const wss = new WebSocket.Server({ port: PORT });

// クライアント管理
let client1 = null;
let client2 = null; //確認にしやすいように名前もっと違うほうがいいかも

// データ保持用
let client1Data = { x: 96, y: 32, life: 3 };
let client2Data = { x: 96, y: 32, life: 3 };

//タイムアウトチェック用
let client1Timeout = null;
let client2Timeout = null;
//10秒
const TIMEOUT_MS = 10000;

// 定数
const WAITING_LIFE = 255;

// マッチング状態
let isMatched = false;
let matchStartTime = null;


// ログ出力用ヘルパー関数
function log(message) {
    if (matchStartTime) {
        const elapsedTime = ((Date.now() - matchStartTime) / 1000).toFixed(3);
        console.log(`[${elapsedTime}s] ${message}`);
    } else {
        console.log(message);
    }
}

log(`WebSocketサーバー起動: ws://localhost:${PORT}`);

wss.on('connection', (ws, req) => {
    const ip = req.socket.remoteAddress;

    // クライアント割り当て
    if (!client1) {
        client1 = ws;
        log(`client1接続: ${ip}`);
        isMatched = false;
        sendWaitingToClient(client1);
        resetTimeout(client1, 'client1');
    } else if (!client2) {
        client2 = ws;
        isMatched = true;
        matchStartTime = Date.now();
        log(`client2接続: ${ip}`);
        sendInitialData();
        resetTimeout(client2, 'client2');
    } else {
        log(`接続拒否（満員）: ${ip}`);
        ws.close(1000, 'サーバー満員');
        return;
    }

    //送信処理
    ws.on('message', (data) => {

        // マッチング中のときだけタイマーをリセットする
        if (ws === client1) {
            resetTimeout(ws, 'client1');
        } else if (ws === client2) {
            resetTimeout(ws, 'client2');
        }

        if (data.length >= 3) {
            const x = data[0];
            const y = data[1];
            const life = data[2];

            const sender = ws === client1 ? 'client1' : 'client2';
            log(`${sender} からデータ受信: x=${x}, y=${y}, life=${life}`);

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

    ws.on('close', () => {
        if (ws === client1) {
            log('client1が切断されました');
            clearTimeout(client1Timeout);
            client1 = null;
            client1Data = { x: 96, y: 32, life: 3 };

            // client2 がいれば通知＆切断
            if (client2 && client2.readyState === WebSocket.OPEN) {
                client2.terminate();
            }
            client2 = null;
            client2Data = { x: 96, y: 32, life: 3 };
        } else if (ws === client2) {
            log('client2が切断されました');
            clearTimeout(client2Timeout);
            client2 = null;
            client2Data = { x: 96, y: 32, life: 3 };

            if (client1 && client1.readyState === WebSocket.OPEN) {
                client1.terminate();
            }
            client1 = null;
            client1Data = { x: 96, y: 32, life: 3 };
        }

        isMatched = false;
        matchStartTime = null;
    });
});


// client1 に life=255 の「待機中」を通知
function sendWaitingToClient(client) {
    if (client && client.readyState === WebSocket.OPEN) {
        const buffer = Buffer.from([client2Data.x, client2Data.y, WAITING_LIFE]);
        client.send(buffer);
        log('client1に「待機中」通知（life=255）');
    }
}

//messageイベント内で呼び出す
function resetTimeout(client, which) {
    const timeoutVar = which === 'client1' ? 'client1Timeout' : 'client2Timeout';

    if (global[timeoutVar]) {
        clearTimeout(global[timeoutVar]);
    }

    global[timeoutVar] = setTimeout(() => {
        log(`${which} 応答なしにより切断`);
        client.terminate(); // 明示的に切断
    }, TIMEOUT_MS);
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
