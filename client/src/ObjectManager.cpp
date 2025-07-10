#include "ObjectManager.h"
#include <utility> // for std::move

//オブジェクトを追加
int ObjectManager::addObj(std::unique_ptr<ObjBase> obj)
{
    for(int i = 0; i < MAX_OBJ; i++)
    {
        //空いている場所に追加
        if(m_objList[i] == nullptr)
        {
            m_objList[i] = std::move(obj); // 所有権をObjectManagerに移動
            return i;//インデックスを返す
        }
    }
    //追加に失敗
    return -1;
}

//オブジェクトを更新
void ObjectManager::updateObj()
{
    for(auto& obj : m_objList) // 範囲ベースforループで簡潔に
    {
        //オブジェクトが存在してかつアクティブなら更新する
        if(obj != nullptr && obj->m_isAlive)
        {
            obj->action();
        }
    }
}

//オブジェクトを描画
void ObjectManager::drawObj()
{
    for(const auto& obj : m_objList) // 描画のみなのでconst参照
    {
        //オブジェクトが存在してかつアクティブなら描画する
        if(obj != nullptr && obj->m_isAlive)
        {
            obj->draw();
        }
    }
}

//オブジェクトをクリーンアップ
void ObjectManager::cleanupObj()
{
    for(auto& obj : m_objList)
    {
        //オブジェクトが非アクティブなら削除
        if(obj != nullptr && !obj->m_isAlive)
        {
            obj.reset(); // スマートポインタをリセットし、管理下のオブジェクトを破棄
        }
    }
}

//指定のオブジェクトのポインタを返す(nullptrに注意する)
// 呼び出し元は所有権を持たない生のポインタを受け取る
ObjBase* ObjectManager::getObjPtr(int8_t index) 
{
    // 範囲外アクセスを防ぐためのチェックを追加
    if (index >= 0 && index < MAX_OBJ) {
        return m_objList[index].get(); // .get()で生のポインタを返す
    }
    return nullptr;
}