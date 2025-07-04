#pragma once
#include "sceneBase.h"

class Title: public SceneBase
{
    private:
    // ゲームのモード
    bool m_vsmode;
    //　ロゴをしばらく表示するためのタイマー
    uint8_t m_timer;
    public:
    Title(SceneManager* p);
    int update();
    void draw();
    ~Title();
};