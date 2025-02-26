#include "Projectile.h"

Projectile::Projectile(float x, float y, float speed, bool isBomb)
    : x(x), y(y), speed(speed), width(8), height(10), isBomb(isBomb), exploded(false) {}

void Projectile::Update()
{
    if (isBomb && !exploded)
    {
        // Bomb explodes at a random point
        if (GetRandomValue(0, 100) < 1)
        {
            exploded = true;
            explosionTime = GetTime();
        }
    }

    if (!exploded)
    {
        // Frame rate independent movement
        float deltaTime = GetFrameTime();
        y += speed * deltaTime * 60.0f; // Positive speed goes down, negative goes up
    }
}

void Projectile::Draw() const
{
    if (exploded)
    {
        // Draw explosion effect
        float elapsedTime = GetTime() - explosionTime;
        if (elapsedTime < 0.5f)
        {
            DrawCircle(static_cast<int>(x), static_cast<int>(y), elapsedTime * 50.0f, ORANGE);
        }
    }
    else
    {
        Color projectileColor = speed < 0 ? BLUE : RED; // Player projectiles blue, enemy projectiles red
        if (isBomb)
        {
            projectileColor = DARKGRAY;                                               // Bombs are dark gray
            DrawCircle(static_cast<int>(x), static_cast<int>(y), 5, projectileColor); // Draw bomb shape
        }
        else
        {
            DrawRectangle(static_cast<int>(x - width / 2), static_cast<int>(y - height / 2),
                          static_cast<int>(width), static_cast<int>(height), projectileColor);
        }
    }
}

Rectangle Projectile::GetBounds() const
{
    return Rectangle{x - width / 2, y - height / 2, width, height}; // Centered bounds
}

bool Projectile::IsOffScreen(int screenHeight) const
{
    return (speed < 0 && y + height < 0) || (speed > 0 && y > screenHeight);
}

bool Projectile::HasExploded() const
{
    return exploded;
}
