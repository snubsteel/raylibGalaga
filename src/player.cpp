#include "Player.h"
#include <cmath> // Provides fmaxf and fminf

Player::Player(float posXPercent, float posYPercent, int width, int height)
    : posXPercent(posXPercent), posYPercent(posYPercent), width(width), height(height), lives(3), exploding(false), explosionTime(0.0f), shootFromLeft(true)
{
    movementSpeed = 7.5f;
    shootCooldown = 0.2f;
    timeSinceLastShot = 0.0f;
}

void Player::Update(int screenWidth, int screenHeight)
{
    if (exploding)
    {
        explosionTime += GetFrameTime();
        return;
    }

    x = static_cast<int>(posXPercent * screenWidth);
    y = static_cast<int>(posYPercent * screenHeight);

    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        posXPercent += movementSpeed / screenWidth;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        posXPercent -= movementSpeed / screenWidth;

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        posYPercent -= movementSpeed / screenHeight;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        posYPercent += movementSpeed / screenHeight;

    // Clamp player's horizontal position so the sprite doesn't go off-screen.
    float leftBound = (width / 2.0f) / screenWidth;
    float rightBound = 1.0f - (width / 2.0f) / screenWidth;
    posXPercent = fmaxf(leftBound, fminf(posXPercent, rightBound));

    if (posYPercent < 0.0f)
        posYPercent = 0.0f;
    if (posYPercent > 1.0f)
        posYPercent = 1.0f;

    // Update the time since the last shot
    timeSinceLastShot += GetFrameTime();

    if ((IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) && timeSinceLastShot >= shootCooldown)
    {
        Shoot();
        timeSinceLastShot = 0.0f; // Reset the timer
    }

    for (auto it = projectiles.begin(); it != projectiles.end();)
    {
        it->Update();
        if (it->IsOffScreen(screenHeight))
        {
            it = projectiles.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Player::Draw(Texture2D playerTexture) const
{
    if (exploding)
    {
        DrawText("BOOM!", x - 20, y - 10, 20, RED);
        return;
    }
    // Scale factor: original scale 0.6 halved -> 0.3
    DrawTextureEx(playerTexture, {static_cast<float>(x - width / 2), static_cast<float>(y - height / 2)}, 0.0f, 0.3f, WHITE);
    for (const auto &projectile : projectiles)
    {
        projectile.Draw();
    }
}

void Player::Shoot()
{
    float projectileY = y - (height * 0.5f);
    // Calculate the left and right cannon positions relative to the player's center
    float leftCannonX = x - (width * 0.5f);
    float rightCannonX = x + (width * 0.01f) - 60;

    // Alternate shooting between left and right cannons
    if (shootFromLeft)
    {
        projectiles.emplace_back(leftCannonX, projectileY, -10.0f * 0.75f); // Negative speed to shoot upward
    }
    else
    {
        projectiles.emplace_back(rightCannonX, projectileY, -10.0f * 0.75f); // Negative speed to shoot upward
    }
    shootFromLeft = !shootFromLeft; // Toggle the flag
}

std::vector<Projectile> &Player::GetProjectiles()
{
    return projectiles;
}

int Player::GetLives() const
{
    return lives;
}

void Player::LoseLife()
{
    lives--;
    exploding = true;
    explosionTime = 0.0f;
    projectiles.clear(); // Clear projectiles when the player loses a life
}

void Player::Respawn()
{
    posXPercent = 0.5f;
    posYPercent = 0.9f;
    exploding = false;
}

int Player::GetX() const
{
    return x;
}

int Player::GetY() const
{
    return y;
}

int Player::GetWidth() const
{
    return static_cast<int>(width * 0.5f); // use scaled width
}

int Player::GetHeight() const
{
    return static_cast<int>(height * 0.5f); // use scaled height
}

bool Player::IsExploding() const
{
    return exploding;
}

float Player::GetExplosionTime() const
{
    return explosionTime;
}