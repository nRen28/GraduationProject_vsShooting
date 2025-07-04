#pragma once
#include "types.h"
#include "objManager.h"

class SceneManager;

//シーンの基底クラス
class SceneBase
{
    protected:
    //シーンマネージャー
    SceneManager* sceneManager{nullptr};
    
    public:
    SceneBase(SceneManager* ptr_sceneManager){sceneManager = ptr_sceneManager;}
    virtual int update() = 0;
    virtual void draw() = 0;
    virtual void communicate() {}  //通信処理
    virtual ~SceneBase() {}
};