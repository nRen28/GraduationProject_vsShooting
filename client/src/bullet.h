#pragma once
#include "objBase.h"

// 弾構造体------------------------------------------------
struct Bullet : public ObjBase
{
public:
  Bullet(Pos _pos, Vec _vec, int8_t _id);
  void action();
  void draw();
  ~Bullet() {}

private:
  //弾の大きさ
  uint8_t m_radius = 2;
  
  // 弾が生成された時間(自然消滅用)
  unsigned long m_createdTime = 0;

  Vec m_direction; // 移動方向
};