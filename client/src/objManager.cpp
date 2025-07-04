#include "objManager.h"

//オブジェクトを追加
int ObjManager::addObj(ObjBase* obj)
{
    for(int i = 0; i < MAX_OBJ; i++)
    {
        //空いている場所に追加
        if(objList[i] == nullptr)
        {
            objList[i] = obj;
            return i;//インデックスを返す
        }
    }
    //追加に失敗
    return -1;
}

//オブジェクトを更新
void ObjManager::updateObj()
{
    for(int i = 0; i < MAX_OBJ; i++)
    {
        //オブジェクトが存在してかつアクティブなら更新する
        if(objList[i] != nullptr && objList[i]->m_isAlive)
        {
            objList[i]->action();
        }
    }
}

//オブジェクトを描画
void ObjManager::drawObj()
{
    for(int i = 0; i < MAX_OBJ; i++)
    {
        //オブジェクトが存在してかつアクティブなら描画する
        if(objList[i] != nullptr && objList[i]->m_isAlive)
        {
            objList[i]->draw();
        }
    }
}

//オブジェクトをクリーンアップ
void ObjManager::cleanupObj()
{
    for(int i = 0; i < MAX_OBJ; i++)
    {
        //オブジェクトが非アクティブなら削除
        if(objList[i] != nullptr && objList[i]->m_isAlive == false)
        {
            delete objList[i];
            objList[i] = nullptr;
        }
    }
}

//指定のオブジェクトのポインタを返す(nullptrに注意する)
ObjBase* ObjManager::getObjPtr(int8_t index)
{
    return objList[index];
}

ObjManager::~ObjManager()
{
    //すべてdeleteしてメモリリークを防ぐ
    for (int i = 0; i < MAX_OBJ; i++)
    {
        if (objList[i] != nullptr)
        {
            delete objList[i];
            objList[i] = nullptr;
        }
    }
}