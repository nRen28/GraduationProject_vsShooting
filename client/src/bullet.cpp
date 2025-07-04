#include "Bullet.h"

Bullet::Bullet(Pos _pos, Vec _direction, int8_t _id)
{
    //初期位置
    m_pos = _pos;
    //大きさ
    m_width = m_radius;
    m_height = m_radius;
    //オブジェクトID
    m_id = _id;

    m_pos = _pos;
    m_id = _id;
    m_direction = _direction;
}

void Bullet::action()
{
    // 方向に応じて移動
    m_pos.x += m_direction.x;
    m_pos.y += m_direction.y;

    //一定時間後に消滅
    m_createdTime++;
    if(m_createdTime >600)
    {
        m_isAlive = false;
    }

    //画面外に出たら消滅
    if (m_pos.x + m_vec.x > SCREEN_WIDTH)
    {
        m_isAlive = false;
    }
    if (m_pos.x + m_vec.x < 0 - m_width)
    {
        m_isAlive = false;
    }
    if (m_pos.y + m_vec.y > SCREEN_HEIGHT)
    {
        m_isAlive = false;
    }
    if (m_pos.y + m_vec.y < 0 - m_height)
    {
        m_isAlive = false;
    }
}

void Bullet::draw()
{
    //オブジェクトIDで弾の見た目を変更
    if(m_id == P1BULLET)
    {
        display.fillCircle(m_pos.x, m_pos.y, m_radius, WHITE);
    }
    else
    {
        display.drawCircle(m_pos.x, m_pos.y, m_radius, WHITE);
    }
}