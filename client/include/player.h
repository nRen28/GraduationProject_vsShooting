#pragma once
#include "objBase.h"
#include "objManager.h"

class Player : public ObjBase
{
public:
    Player(Pos pos, int8_t id, ObjManager *_ptr_objManager);
    void action();
    void draw();
    ~Player() {}

    // 当たり判定を調べる
    bool isHitByEnemyBullet();

    uint8_t get_life(){return m_life;}
    void set_life(uint8_t _life){m_life = _life;}

private:
    ObjManager *m_ptr_objManager;

    //弾の発射数
    uint16_t m_bulletCount = 1;
    //弾発射に使うタイマー
    uint8_t m_firetimer = 0;
    //時間経過で発射する弾の数を増やすためのタイマー
    uint16_t m_powerUpTimer = 0;
    //プレイヤーの残機
    uint8_t m_life = 2;
    //相手プレイヤーの弾のID
    uint8_t m_enemyBulletId = 0;
};