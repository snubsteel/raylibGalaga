#include "enemy.h"
#include <cmath>

Enemy::Enemy(float spawnX, float spawnY, float formationX, float formationY,
             float speed, EnemyType type, int formationSlot)
    : x(spawnX), y(spawnY), spawnX(spawnX), spawnY(spawnY), formationX(formationX), formationY(formationY),
      speed(speed), attackTimer(0.0f), diveProgress(0.0f), waveOffset(formationSlot * 0.35f),
      type(type), state(ENTERING), health(1), formationSlot(formationSlot)
{
    switch (type)
    {
    case DRONE:
        bodyColor = RED;
        health = 1;
        attackCooldown = 4.5f;
        break;
    case BEE:
        bodyColor = ORANGE;
        health = 2;
        attackCooldown = 3.8f;
        break;
    case BOSS:
        bodyColor = PURPLE;
        health = 4;
        attackCooldown = 3.0f;
        break;
    }
}

void Enemy::Update(float formationPhase)
{
    UpdateState();
    UpdateMovement(formationPhase);

    attackTimer += GetFrameTime();
    if (attackTimer >= attackCooldown && state != ENTERING)
    {
        Shoot();
        attackTimer = 0.0f;
    }

    UpdateProjectiles();
}

void Enemy::UpdateState()
{
    if (state == ENTERING)
    {
        const float dx = formationX - x;
        const float dy = formationY - y;
        if ((dx * dx + dy * dy) < 64.0f)
        {
            state = FORMATION;
            x = formationX;
            y = formationY;
        }
        return;
    }

    if (state == FORMATION)
    {
        float diveChance = 0.0005f;
        if (type == BEE)
            diveChance = 0.0010f;
        if (type == BOSS)
            diveChance = 0.0014f;

        if (GetRandomValue(0, 10000) < static_cast<int>(diveChance * 10000.0f))
        {
            state = DIVING;
            diveProgress = 0.0f;
        }
        return;
    }

    if (state == DIVING && y > GetScreenHeight() + 40)
    {
        state = ENTERING;
        x = spawnX;
        y = spawnY;
    }
}

void Enemy::UpdateMovement(float formationPhase)
{
    const float delta = GetFrameTime() * 60.0f;

    if (state == ENTERING)
    {
        float t = fminf(1.0f, speed * 0.012f);
        x += (formationX - x) * t;
        y += (formationY - y) * t;
        return;
    }

    if (state == FORMATION)
    {
        float swayX = sinf(formationPhase + waveOffset) * 22.0f;
        float swayY = cosf(formationPhase * 0.7f + waveOffset) * 10.0f;
        x += (formationX + swayX - x) * 0.08f;
        y += (formationY + swayY - y) * 0.08f;
        return;
    }

    diveProgress += 0.035f * (speed / 2.0f);
    float arc = sinf(diveProgress * 6.28318f + waveOffset);

    float horizontalSpeed = 3.0f;
    if (type == BEE)
        horizontalSpeed = 4.5f;
    if (type == BOSS)
        horizontalSpeed = 2.5f;

    x += arc * horizontalSpeed * delta;
    y += speed * 1.5f * delta;

    if (x < 20)
        x = 20;
    if (x > GetScreenWidth() - 20)
        x = GetScreenWidth() - 20;
}

void Enemy::UpdateProjectiles()
{
    for (auto it = projectiles.begin(); it != projectiles.end();)
    {
        it->Update();
        if (it->IsOffScreen(GetScreenHeight()))
            it = projectiles.erase(it);
        else
            ++it;
    }
}

void Enemy::Draw() const
{
    int size = 20;
    if (type == BEE)
        size = 24;
    if (type == BOSS)
        size = 30;

    DrawRectangle(static_cast<int>(x) - size / 2, static_cast<int>(y) - size / 2, size, size, bodyColor);

    for (int i = 0; i < health; ++i)
        DrawRectangle(static_cast<int>(x) - size / 2 + 2 + i * 7, static_cast<int>(y) - size / 2 - 7, 5, 3, GREEN);

    for (const auto &projectile : projectiles)
        projectile.Draw();
}

bool Enemy::IsOffScreen(int screenHeight) const
{
    return y > screenHeight + 40;
}

Rectangle Enemy::GetBounds() const
{
    float size = 20.0f;
    if (type == BEE)
        size = 24.0f;
    if (type == BOSS)
        size = 30.0f;

    return {x - size / 2.0f, y - size / 2.0f, size, size};
}

void Enemy::TakeDamage()
{
    health--;
}

bool Enemy::IsDestroyed() const
{
    return health <= 0;
}

int Enemy::GetScoreValue() const
{
    if (type == DRONE)
        return 80;
    if (type == BEE)
        return 140;
    return 260;
}

void Enemy::Shoot()
{
    float projectileSpeed = 5.0f;
    bool bomb = false;

    if (type == BEE)
        projectileSpeed = 6.0f;
    if (type == BOSS)
    {
        projectileSpeed = 4.0f;
        bomb = true;
    }

    projectiles.emplace_back(x, y + 20, projectileSpeed, bomb);
}

const std::vector<Projectile> &Enemy::GetProjectiles() const
{
    return projectiles;
}

std::vector<Projectile> &Enemy::GetProjectiles()
{
    return projectiles;
}
