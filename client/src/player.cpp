#include "player.h"
#include "bullet.h"
#include "GameUtils.h"
#include <memory> // std::make_unique のために追加

static constexpr int kFireRate = 50;
static constexpr int kJoystickThresholdHigh = 800;
static constexpr int kJoystickThresholdLow = 400;

Player::Player(Pos _pos, int8_t _id, ObjManager *_ptr_objManager)
{
    // 初期位置
    m_pos = _pos;
    // サイズ
    m_width = 7;
    m_height = 7;

    // オブジェクトID
    m_id = _id;

    // オブジェクトマネージャーのポインタ
    m_ptr_objManager = _ptr_objManager;

    // 相手プレイヤーの弾のIDを設定
    if (m_id == PLAYER1)
    {
        m_enemyBulletId = P2BULLET;
    }
    else
    {
        m_enemyBulletId = P1BULLET;
    }
}

void Player::action()
{
    m_vec.x = 0;
    m_vec.y = 0;

    if (m_id == PLAYER1)
    {
        // 移動処理
        if (kJoystickThresholdHigh < analogRead(STICK_X))
            m_vec.x = 1;
        if (kJoystickThresholdLow > analogRead(STICK_X))
            m_vec.x = -1;
        if (kJoystickThresholdHigh < analogRead(STICK_Y))
            m_vec.y = 1;
        if (kJoystickThresholdLow > analogRead(STICK_Y))
            m_vec.y = -1;

        // 正規化
        float magnitude = sqrt(m_vec.x * m_vec.x + m_vec.y * m_vec.y);
        if (magnitude != 0)
        {
            m_vec.x = m_vec.x / magnitude;
            m_vec.y = m_vec.y / magnitude;
        }
    }

    // 弾生成
    m_firetimer++;
    if (m_firetimer >= kFireRate)
    {
        int bulletId = (m_id == PLAYER1) ? P1BULLET : P2BULLET;

        for (int i = 0; i < m_bulletCount; i++)
        {
            float baseAngle, spreadRange;

            if (m_id == PLAYER1) // 右向き（右側90度）
            {
                baseAngle = 0;        // 右向きを基準（0度）
                spreadRange = PI / 2; // 90度の範囲
            }
            else // PLAYER2 左向き（左側90度）
            {
                baseAngle = PI;       // 左向きを基準（180度）
                spreadRange = PI / 2; // 90度の範囲
            }

            // 弾が1発の場合は真っ直ぐ、複数の場合は範囲内に分散
            float angle;
            if (m_bulletCount == 1)
            {
                angle = baseAngle; // 真っ直ぐ
            }
            else
            {
                // 範囲内に均等に分散（-45度から+45度の範囲）
                float offset = (spreadRange * (i - (m_bulletCount - 1) / 2.0)) / (m_bulletCount - 1);
                angle = baseAngle + offset;
            }

            Vec dir = {
                cos(angle) * 0.8, // X方向の速度
                sin(angle) * 0.8  // Y方向の速度
            };
            m_ptr_objManager->addObj(std::make_unique<Bullet>(Pos{m_pos.x + m_width / 2, m_pos.y + m_height / 2}, dir, bulletId));
        }

        m_firetimer = 0;
    }

    // 時間経過で発射する弾の数UP
    m_powerUpTimer++;
    if (m_powerUpTimer == 600 && m_bulletCount < 15)
    {
        m_bulletCount++;
        m_powerUpTimer = 0;
    }

    // 画面外に出ないように制限
    if (m_id == PLAYER1)
    {
        if (m_pos.x + m_vec.x > SCREEN_WIDTH / 2 - m_width) // 画面中央より先には進めない
        {
            m_pos.x = SCREEN_WIDTH / 2 - m_width;
            m_vec.x = 0;
        }
        if (m_pos.x + m_vec.x < 0)
        {
            m_pos.x = 0;
            m_vec.x = 0;
        }
        if (m_pos.y + m_vec.y > SCREEN_HEIGHT - m_height)
        {
            m_pos.y = SCREEN_HEIGHT - m_height;
            m_vec.y = 0;
        }
        if (m_pos.y + m_vec.y < 0)
        {
            m_pos.y = 0;
            m_vec.y = 0;
        }
    }

    // ベクトルをポジションに反映
    m_pos.x += m_vec.x;
    m_pos.y += m_vec.y;

    // 弾と当たったら残機を減らす
    if (isHitByEnemyBullet())
    {
        if (m_life == 0)
        {
            m_isAlive = false;
        }
        else
        {
            m_life--;
        }
    }
}

void Player::draw()
{
    // プレイヤーを描画
    if (m_id == PLAYER1)
    {
        display.fillRect(m_pos.x, m_pos.y, m_width, m_height, WHITE);
    }
    else
    {
        display.drawRect(m_pos.x, m_pos.y, m_width, m_height, WHITE);
    }
}

// 弾との当たり判定
bool Player::isHitByEnemyBullet()
{
    for (int i = 0; i < MAX_OBJ; i++)
    {
        // オブジェクトのポインタ
        ObjBase *ptr_obj = m_ptr_objManager->getObjPtr(i);
        // オブジェクトが存在するかチェック
        if (ptr_obj != nullptr && ptr_obj->m_isAlive)
        {
            // オブジェクトのIDを調べる
            if (ptr_obj->m_id == m_enemyBulletId)
            {
                int targetX = ptr_obj->m_pos.x;
                int targetY = ptr_obj->m_pos.y;
                int targetWidth = ptr_obj->m_width;
                int targetHeight = ptr_obj->m_height;

                // 弾と当たっていたら弾を消す
                if (isColliding(m_pos.x, m_pos.y, m_width, m_height, targetX, targetY, targetWidth, targetHeight))
                {
                    ptr_obj->m_isAlive = false;
                    return true;
                }
            }
        }
    }
    return false;
}