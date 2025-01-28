#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <raylib.h>

class Projectile {
public:
    Projectile(float x, float y, float speed);
    void Update();
    void Draw() const;
    Rectangle GetBounds() const;
    bool IsOffScreen(int screenHeight) const;

private:
    float x;
    float y;
    float speed;
    float width;
    float height;
};

#endif // PROJECTILE_H