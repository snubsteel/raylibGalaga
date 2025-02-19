#include "Enemy.h"
#include <cmath> // Include cmath for sin and cos functions

Enemy::Enemy(float x, float y, float speed)
    : x(x), y(y), speed(speed), shootCooldown(2.0f), timeSinceLastShot(0.0f), movementPatternTime(0.0f), state(ENTERING) {
    color = RED;
}

void Enemy::Update() {
    UpdateState();
    UpdateMovementPattern();

    // Update the time since the last shot
    timeSinceLastShot += GetFrameTime();

    if (timeSinceLastShot >= shootCooldown) {
        Shoot();
        timeSinceLastShot = 0.0f; // Reset the timer
    }

    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        it->Update();
        if (it->IsOffScreen(GetScreenHeight())) {
            it = projectiles.erase(it);
        } else {
            ++it;
        }
    }
}

void Enemy::UpdateState() {
    switch (state) {
        case ENTERING:
            y += speed;
            if (y > 100) { // Example threshold for entering state
                state = FORMATION;
            }
            break;
        case FORMATION:
            // Stay in formation for a while
            if (movementPatternTime > 5.0f) { // Example time to stay in formation
                state = ATTACKING;
                movementPatternTime = 0.0f;
            }
            break;
        case ATTACKING:
            y += speed * 2; // Attack downwards
            x += cos(movementPatternTime * speed) * 5.0f; // Spiral downwards
            break;
    }
}

void Enemy::UpdateMovementPattern() {
    movementPatternTime += GetFrameTime();
    if (state == FORMATION) {
        x += sin(movementPatternTime * speed) * 2.0f; // Example movement pattern
    }
}

void Enemy::Draw() const {
    // Draw enemy with half size: 20x20 instead of 40x40
    DrawRectangle(static_cast<int>(x) - 10, static_cast<int>(y) - 10, 20, 20, color);
    for (const auto& projectile : projectiles) {
        projectile.Draw();
    }
}

bool Enemy::IsOffScreen(int screenHeight) const {
    return y > screenHeight;
}

Rectangle Enemy::GetBounds() const {
    // Update bounds to match half size enemy sprite
    return { x - 10, y - 10, 20, 20 };
}

void Enemy::Shoot() {
    projectiles.emplace_back(x, y + 20, -5.0f); // Shoot downwards
}

const std::vector<Projectile>& Enemy::GetProjectiles() const {
    return projectiles;
}

std::vector<Projectile>& Enemy::GetProjectiles() {
    return projectiles;
}