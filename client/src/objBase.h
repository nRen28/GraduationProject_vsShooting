#pragma once
#include "types.h"

// オブジェクトの基底クラス
class ObjBase
{
public:
    // 座標
    Pos m_pos{0, 0};
    // ベクトル
    Vec m_vec{0, 0};
    // 生存フラグ
    bool m_isAlive = true;
    // 横幅
    uint8_t m_width = 1;
    // 縦幅
    uint8_t m_height = 1;
    // オブジェクトID
    int8_t m_id = -1;

    virtual void action() = 0;

    virtual void draw() = 0;
};