#include "Player.h"

Player::Player(float posXPercent, float posYPercent, int width, int height)
    : posXPercent(posXPercent), posYPercent(posYPercent), width(width), height(height) {
    movementSpeed = 15.0f;
    shootCooldown = 0.2f;
    timeSinceLastShot = 0.0f;
}

void Player::Update(int screenWidth, int screenHeight) {
    x = static_cast<int>(posXPercent * screenWidth);
    y = static_cast<int>(posYPercent * screenHeight);

    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) posXPercent += movementSpeed / screenWidth;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) posXPercent -= movementSpeed / screenWidth;

    if (posXPercent < 0.0f) posXPercent = 0.0f;
    if (posXPercent > 1.0f) posXPercent = 1.0f;

    // Update the time since the last shot
    timeSinceLastShot += GetFrameTime();

    if ((IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) && timeSinceLastShot >= shootCooldown) {
        Shoot();
        timeSinceLastShot = 0.0f; // Reset the timer
    }

    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        it->Update();
        if (it->IsOffScreen(screenHeight)) {
            it = projectiles.erase(it);
        } else {
            ++it;
        }
    }
}

void Player::Draw() const {
    DrawRectangle(x - width / 2, y - height / 2, width, height, WHITE);
    for (const auto& projectile : projectiles) {
        projectile.Draw();
    }
}

void Player::Shoot() {
    projectiles.emplace_back(x, y - height / 2, 10.0f);
}

std::vector<Projectile>& Player::GetProjectiles() {
    return projectiles;
}