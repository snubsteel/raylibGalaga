#include "Enemy.h"

Enemy::Enemy(float x, float y, float speed)
    : x(x), y(y), speed(speed) {
    color = RED;
}

void Enemy::Update() {
    y += speed;
}

void Enemy::Draw() const {
    DrawRectangle(static_cast<int>(x) - 20, static_cast<int>(y) - 20, 40, 40, color);
}

bool Enemy::IsOffScreen(int screenHeight) const {
    return y > screenHeight;
}

Rectangle Enemy::GetBounds() const {
    return { x - 20, y - 20, 40, 40 };
}