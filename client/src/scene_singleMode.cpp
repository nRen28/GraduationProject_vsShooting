#include <EEPROM.h>
#include "function.h"
#include "scene_singleMode.h"
#include "sceneManager.h"
#include "scene_title.h"
#include "player.h"
#include "enemy.h"
#include "effect.h"

#define COUNTDOWN_DURATION 60
#define STAR_GEN_DURATION 24

SingleMode::SingleMode(SceneManager *p) : SceneBase(p)
{
    // フラッシュメモリからハイスコアをロード
    m_hiscore = EEPROM.read(EEPROM_ADDR_HISCORE);

    randomSeed(analogRead(0)); // エネミーのランダム移動用　0番ピンのノイズで乱数を初期化

    // ゲームの状態を初期化

    m_gameState = STATE_COUNTDOWN;

    // カウントダウンの音を鳴らすためにあえて4秒から開始
    m_countDown = 3 + 1;
    m_timer = 60;

    m_score = 0;
    m_p1Life = 3;
    m_p1death = false;

    // プレイヤーとエネミー生成
    m_objManager.addObj(new Player({0, SCREEN_HEIGHT / 2 - 5}, PLAYER1, &m_objManager));
    m_objManager.addObj(new Enemy(&m_objManager));

    // あらかじめ画面にランダムに星を出しておく
    for (int i = 0; i < 3; ++i)
    {
        // ランダムな位置に出現する（x, y ともにランダム）
        float x = static_cast<float>(randRange(0, SCREEN_WIDTH));
        float y = static_cast<float>(randRange(0, SCREEN_HEIGHT));
        Pos pos{x, y};

        // 左に流れていく
        Vec vec{-1.0f, 0.0f};

        m_objManager.addObj(new Effect(pos, vec));
    }
}

int SingleMode::update()
{
    // ゲームの状態
    switch (m_gameState)
    {
    // 開始前のカウントダウン
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
                if (m_countDown == 0)
                {
                    // カウントダウン終了
                    sound.playSound(SOUND_COUNTDOWN_END);
                }
                else
                {
                    // カウントダウン
                    sound.playSound(SOUND_COUNTDOWN_TICK);
                }
            }
            m_timer = 0;
        }
        m_timer++;
        break;
    // ゲーム中
    case STATE_IN_GAME:
        // プレイヤーが死ぬまで続ける
        if (!m_p1death)
        {
            // 画面に星のエフェクトを生成
            if (m_starGenTimer == STAR_GEN_DURATION)
            {
                //  ランダムな縦位置に出現する
                float y = static_cast<float>(randRange(0, SCREEN_HEIGHT));
                Pos pos{SCREEN_WIDTH, y};
                // 左に流れていく
                Vec vec{-1.0f, 0.0f};

                m_objManager.addObj(new Effect(pos, vec));

                m_starGenTimer = 0;
            }
            m_starGenTimer++;

            // オブジェクト更新
            m_objManager.updateObj();

            // オブジェクトクリーンアップ
            m_objManager.cleanupObj();

            // プレイヤーが死んだか判定用
            m_p1death = true;
            // 画面上に表示する残機とスコア更新
            for (int i = 0; i < MAX_OBJ; i++)
            {
                if (m_objManager.getObjPtr(i) != nullptr && m_objManager.getObjPtr(i)->m_id == PLAYER1)
                {
                    m_p1Life = ((Player *)m_objManager.getObjPtr(i))->get_life();
                    m_p1death = false;
                }

                if (m_objManager.getObjPtr(i) != nullptr && m_objManager.getObjPtr(i)->m_id == ENEMY)
                {
                    m_score = ((Enemy *)m_objManager.getObjPtr(i))->get_hitCount();
                }
            }
        }
        else
        {
            // 結果画面へ
            m_gameState = STATE_RESULT;
            // ゲームオーバーBGM
            sound.playSound(SOUND_LOSS);
        }
        break;
    // 結果画面
    case STATE_RESULT:
        // タイトルに戻る
        if (digitalRead(BUTTON_A) == HIGH)
        {
            // ハイスコアならフラッシュメモリに保存
            if (m_hiscore < m_score)
            {
                m_hiscore = m_score;
                EEPROM.write(EEPROM_ADDR_HISCORE, (byte)m_hiscore);
            }
            sound.stopSound();
            sceneManager->deleteScene();
            sceneManager->currentScene = new Title(sceneManager);
        }
        break;
    }
    return GAME_RUNNING;
}

void SingleMode::draw()
{
    // ゲームの状態に応じて描画
    switch (m_gameState)
    {
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
        m_objManager.drawObj();

        // UI---------------------------------------------------------------------------------
        display.setTextSize(1);                       // 文字サイズ（1が基本サイズ）
        display.setTextColor(WHITE);                  // 文字色
        display.setCursor(0, 0);                      //(x, y) = (0, 0) は1行目の左端
        display.print("P1 LIFE:" + String(m_p1Life)); // 残機を表示
        display.setCursor(67, 0);
        display.print("S:" + String(m_score));
        display.setCursor(98, 0);
        display.print("H:" + String(m_hiscore));
        break;
    // 結果画面
    case STATE_RESULT:
        display.setTextSize(2);
        display.setCursor(0, 20);
        display.print("GAME OVER!");

        display.setTextSize(1);
        display.setCursor(0, 45);
        display.print("Press a button to\n return");
        break;
    }
}

// ここは何の目的でおいてますか？
SingleMode::~SingleMode()
{
}