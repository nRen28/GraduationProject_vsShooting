#include "function.h"

//当たり判定をチェック
bool isColliding(int x1, int y1, int w1, int h1,
                 int x2, int y2, int w2, int h2) {
    return x1 + w1 > x2 && x1 < x2 + w2 &&
           y1 + h1 > y2 && y1 < y2 + h2;
}
// 範囲付き乱数関数
int randRange(int min, int max) {
    return rand() % (max - min + 1) + min;
}