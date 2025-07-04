#pragma once
#include "types.h"

//当たり判定をチェック
bool isColliding(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
// 範囲付き乱数関数
int randRange(int min, int max);