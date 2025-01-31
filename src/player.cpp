#include "Player.h"

Player::Player(float posXPercent, float posYPercent, int width, int height)
    : posXPercent(posXPercent), posYPercent(posYPercent), width(width), height(height), lives(3), exploding(false), explosionTime(0.0f), shootFromLeft(true) {
    movementSpeed = 15.0f;
    shootCooldown = 0.2f;
    timeSinceLastShot = 0.0f;
}

void Player::Update(int screenWidth, int screenHeight) {
    if (exploding) {
        explosionTime += GetFrameTime();
        if (explosionTime >= 3.0f) {
            Respawn();
        }
        return;
    }

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

void Player::Draw(Texture2D playerTexture) const {
    if (exploding) {
        DrawText("BOOM!", x - 20, y - 10, 20, RED);
        return;
    }

    // Scale the sprite to 60% of its original size
    DrawTextureEx(playerTexture, { static_cast<float>(x - width / 2), static_cast<float>(y - height / 2) }, 0.0f, 0.6f, WHITE);
    for (const auto& projectile : projectiles) {
        projectile.Draw();
    }
}

void Player::Shoot() {
    float offset = shootFromLeft ? -width / 2.5f : width / 18;
    projectiles.emplace_back(x + offset, y - height / 2 + 10, 10.0f); // Adjust the y position to be near the cannons
    shootFromLeft = !shootFromLeft; // Alternate the shooting side
}

std::vector<Projectile>& Player::GetProjectiles() {
    return projectiles;
}

int Player::GetLives() const {
    return lives;
}

void Player::LoseLife() {
    lives--;
    exploding = true;
    explosionTime = 0.0f;
    projectiles.clear(); // Clear projectiles when the player loses a life
}

void Player::Respawn() {
    posXPercent = 0.5f;
    posYPercent = 0.9f;
    exploding = false;
}

int Player::GetX() const {
    return x;
}

int Player::GetY() const {
    return y;
}

int Player::GetWidth() const {
    return width;
}

int Player::GetHeight() const {
    return height;
}

bool Player::IsExploding() const {
    return exploding;
}