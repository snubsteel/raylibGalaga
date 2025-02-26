#include "Enemy.h"
#include <cmath> // Include cmath for sin and cos functions

Enemy::Enemy(float x, float y, float speed, Color color)
    : x(x), y(y), speed(speed), color(color), shootCooldown(2.0f), timeSinceLastShot(0.0f), movementPatternTime(0.0f), state(ENTERING)
{
    // Set health based on color
    if (color.r == RED.r && color.g == RED.g && color.b == RED.b && color.a == RED.a)
        health = 1;
    else if (color.r == ORANGE.r && color.g == ORANGE.g && color.b == ORANGE.b && color.a == ORANGE.a)
        health = 2;
    else if (color.r == PINK.r && color.g == PINK.g && color.b == PINK.b && color.a == PINK.a)
        health = 3;
    else if (color.r == PURPLE.r && color.g == PURPLE.g && color.b == PURPLE.b && color.a == PURPLE.a)
        health = 3;
}

void Enemy::Update()
{
    UpdateState();
    UpdateMovementPattern();

    // Update the time since the last shot
    timeSinceLastShot += GetFrameTime();

    if (timeSinceLastShot >= shootCooldown && state != ENTERING)
    {
        Shoot();
        timeSinceLastShot = 0.0f; // Reset the timer
    }

    for (auto it = projectiles.begin(); it != projectiles.end();)
    {
        it->Update();
        if (it->IsOffScreen(GetScreenHeight()))
        {
            it = projectiles.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void Enemy::UpdateState()
{
    float deltaTime = GetFrameTime();

    switch (state)
    {
    case ENTERING:
        y += speed * deltaTime * 60.0f; // Frame rate independent movement
        if (y > 100)
        { // Example threshold for entering state
            state = FORMATION;
        }
        break;
    case FORMATION:
        // Stay in formation for a while
        if (movementPatternTime > 10.0f)
        { // Longer formation time
            state = ATTACKING;
            movementPatternTime = 0.0f;
        }
        break;
    case ATTACKING:
        // More complex attack pattern
        y += speed * deltaTime * 40.0f; // Slower descent

        // Swinging left and right while attacking
        x += cos(movementPatternTime * 2.0f) * speed * deltaTime * 100.0f;

        // Ensure enemies don't go off-screen horizontally
        if (x < 20)
            x = 20;
        if (x > GetScreenWidth() - 20)
            x = GetScreenWidth() - 20;
        break;
    }
}

void Enemy::UpdateMovementPattern()
{
    movementPatternTime += GetFrameTime();
    if (state == FORMATION)
    {
        // More complex formation movement
        x += sin(movementPatternTime * speed) * 1.5f; // Gentle swaying
    }
}

void Enemy::Draw() const
{
    // Draw enemy with half size: 20x20 instead of 40x40
    DrawRectangle(static_cast<int>(x) - 10, static_cast<int>(y) - 10, 20, 20, color);

    // Draw health indicator
    for (int i = 0; i < health; i++)
    {
        DrawRectangle(static_cast<int>(x) - 8 + i * 8, static_cast<int>(y) - 15, 5, 2, GREEN);
    }

    for (const auto &projectile : projectiles)
    {
        projectile.Draw();
    }
}

bool Enemy::IsOffScreen(int screenHeight) const
{
    return y > screenHeight;
}

Rectangle Enemy::GetBounds() const
{
    // Update bounds to match half size enemy sprite
    return {x - 10, y - 10, 20, 20};
}

void Enemy::Shoot()
{
    if (color.r == PURPLE.r && color.g == PURPLE.g && color.b == PURPLE.b && color.a == PURPLE.a)
    {
        // PURPLE enemies shoot bombs
        projectiles.emplace_back(x, y + 20, 3.0f, true); // Bombs have a slower speed and are marked as bombs
    }
    else
    {
        projectiles.emplace_back(x, y + 20, 5.0f); // Shoot downwards with positive speed
    }
}

const std::vector<Projectile> &Enemy::GetProjectiles() const
{
    return projectiles;
}

std::vector<Projectile> &Enemy::GetProjectiles()
{
    return projectiles;
}

void Enemy::TakeDamage()
{
    health--;
}

bool Enemy::IsDestroyed() const
{
    return health <= 0;
}
