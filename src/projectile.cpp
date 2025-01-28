#include "Projectile.h"

Projectile::Projectile(float x, float y, float speed)
    : x(x), y(y), speed(speed), width(10), height(20) {}

void Projectile::Update() {
    y -= speed;
}

void Projectile::Draw() const {
    DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(height), WHITE);
}

Rectangle Projectile::GetBounds() const {
    return Rectangle{x, y, width, height};
}

bool Projectile::IsOffScreen(int screenHeight) const {
    return y + height < 0;
}