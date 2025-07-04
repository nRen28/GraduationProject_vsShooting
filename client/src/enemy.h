#pragma once
#include "objBase.h"
#include "objManager.h"

class Enemy: public ObjBase
{
    public:
    Enemy(ObjManager *_ptr_objManager);
    void action();
    void draw();
    ~Enemy() {}

    // 当たり判定を調べる
    bool isHitByEnemyBullet();

    uint8_t get_hitCount(){return m_hitCount;}

    private:
    ObjManager *m_ptr_objManager;

    //弾の発射数
    uint16_t m_bulletCount = 1;
    //弾発射に使うタイマー
    uint8_t m_firetimer = 0;
    //時間経過で発射する弾の数を増やすためのタイマー
    uint16_t m_powerUpTimer = 0;
    //弾に当たった回数
    uint8_t m_hitCount = 0;
};