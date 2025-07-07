#pragma once
#include "objBase.h"

// オブジェクト管理クラス
class ObjManager
{
public:
    ObjManager() {};
    // オブジェクトを追加
    int addObj(ObjBase *obj);
    // オブジェクトを更新
    void updateObj();
    // オブジェクトを描画
    void drawObj();
    // オブジェクトをクリーンアップ
    void cleanupObj();
    ~ObjManager();
    //指定のオブジェクトのポインタを返す(nullptrに注意する)
    ObjBase* getObjPtr(int8_t index);

private:
    // オブジェクトリスト
    ObjBase *objList[MAX_OBJ] = {nullptr};
};