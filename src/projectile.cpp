#include "Projectile.h"

Projectile::Projectile(float x, float y, float speed)
    : x(x), y(y), speed(speed), width(8), height(10) {} 

void Projectile::Update() {
    y -= speed;
}

void Projectile::Draw() const {
    DrawRectangle(static_cast<int>(x - width / 2), static_cast<int>(y - height / 2), static_cast<int>(width), static_cast<int>(height), WHITE); // Centered drawing
}

Rectangle Projectile::GetBounds() const {
    return Rectangle{x - width / 2, y - height / 2, width, height}; // Centered bounds
}

bool Projectile::IsOffScreen(int screenHeight) const {
    return y + height < 0;
}