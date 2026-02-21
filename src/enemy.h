#ifndef ENEMY_H
#define ENEMY_H

#include <raylib.h>
#include <vector>
#include "projectile.h"

enum EnemyState
{
    ENTERING,
    FORMATION,
    DIVING
};

enum EnemyType
{
    DRONE,
    BEE,
    BOSS
};

class Enemy
{
public:
    Enemy(float spawnX, float spawnY, float formationX, float formationY,
          float speed, EnemyType type, int formationSlot);

    void Update(float formationPhase);
    void Draw() const;

    bool IsOffScreen(int screenHeight) const;
    Rectangle GetBounds() const;

    void TakeDamage();
    bool IsDestroyed() const;
    int GetScoreValue() const;

    const std::vector<Projectile> &GetProjectiles() const;
    std::vector<Projectile> &GetProjectiles();

private:
    float x;
    float y;
    float spawnX;
    float spawnY;
    float formationX;
    float formationY;
    float speed;
    float attackTimer;
    float attackCooldown;
    float diveProgress;
    float waveOffset;

    EnemyType type;
    EnemyState state;
    int health;
    int formationSlot;

    Color bodyColor;
    std::vector<Projectile> projectiles;

    void UpdateState();
    void UpdateMovement(float formationPhase);
    void UpdateProjectiles();
    void Shoot();
};

#endif // ENEMY_H
