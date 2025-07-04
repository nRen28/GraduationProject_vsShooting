#include "Enemy.h"
#include "bullet.h"
#include "function.h"

#define FIRERATE 90

Enemy::Enemy(ObjManager *_ptr_objManager)
{
    // 初期位置
    m_pos.x = SCREEN_WIDTH - 30;
    m_pos.y = SCREEN_HEIGHT / 2;
    // サイズ
    m_width = 7;
    m_height = 7;

    // オブジェクトID
    m_id = ENEMY;

    // オブジェクトマネージャーのポインタ
    m_ptr_objManager = _ptr_objManager;
}

void Enemy::action()
{
    m_firetimer++;
    if (m_firetimer % FIRERATE == 0)
    {
        for (int i = 0; i < m_bulletCount; i++)
        {
            float baseAngle, spreadRange;

            baseAngle = PI;       // 左向きを基準（180度）
            spreadRange = PI / 2; // 90度の範囲

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
            m_ptr_objManager->addObj(new Bullet({m_pos.x + m_width / 2, m_pos.y + m_height / 2}, dir, P2BULLET));
        }
    }
    if (m_firetimer == FIRERATE * 2)
    {
        // ランダムにワープ
        m_pos.x = randRange(SCREEN_WIDTH / 2, SCREEN_WIDTH - m_width);
        m_pos.y = randRange(10, SCREEN_HEIGHT - m_height);

        m_firetimer = 0;
    }

    // 時間経過で発射する弾の数UP
    m_powerUpTimer++;
    if (m_powerUpTimer == 600 && m_bulletCount < 15)
    {
        m_bulletCount++;
        m_powerUpTimer = 0;
    }

    // 弾と当たったらカウント
    if (isHitByEnemyBullet())
    {
        m_hitCount++;
    }
}

void Enemy::draw()
{
    // 描画
    display.drawRect(m_pos.x, m_pos.y, m_width, m_height, WHITE);
}

// 弾との当たり判定
bool Enemy::isHitByEnemyBullet()
{
    for (int i = 0; i < MAX_OBJ; i++)
    {
        // オブジェクトのポインタ
        ObjBase *ptr_obj = m_ptr_objManager->getObjPtr(i);
        // オブジェクトが存在するかチェック
        if (ptr_obj != nullptr && ptr_obj->m_isAlive)
        {
            // オブジェクトのIDを調べる
            if (ptr_obj->m_id == P1BULLET)
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