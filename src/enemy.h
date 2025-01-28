#ifndef ENEMY_H
#define ENEMY_H

#include <raylib.h>

class Enemy {
public:
    Enemy(float x, float y, float speed);
    void Update();
    void Draw() const;
    bool IsOffScreen(int screenHeight) const;
    Rectangle GetBounds() const;

private:
    float x;
    float y;
    float speed;
    Color color;
};

#endif // ENEMY_H