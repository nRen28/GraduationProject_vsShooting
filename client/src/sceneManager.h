#pragma once
#include "sceneBase.h"

class SceneManager
{
public:
    SceneManager() {}

    // 現在のシーンのポインタ
    SceneBase *currentScene{nullptr};

    // シーンを更新
    int updateScene() { return currentScene->update(); }
    // シーンを描画
    void drawScene() { currentScene->draw(); }
    // シーンの通信処理
    void communicateScene() { currentScene->communicate(); }

    // シーンを削除
    void deleteScene()
    {
        if (currentScene != nullptr)//nullptrじゃないか念のため確認
        {
            delete currentScene;
            currentScene = nullptr;
        }
    }

    ~SceneManager() { deleteScene(); }
};