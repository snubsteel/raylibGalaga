#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <raylib.h>

class Projectile
{
public:
    Projectile(float x, float y, float speed, bool isBomb = false);
    void Update();
    void Draw() const;
    Rectangle GetBounds() const;
    bool IsOffScreen(int screenHeight) const;
    bool HasExploded() const;

private:
    float x;
    float y;
    float speed;
    float width;
    float height;
    bool isBomb;
    bool exploded;
    double explosionTime;
};

#endif // PROJECTILE_H