#ifndef ENEMY_H
#define ENEMY_H

#include <raylib.h>
#include <vector>
#include "Projectile.h"

enum EnemyState
{
    ENTERING,
    FORMATION,
    ATTACKING
};

class Enemy
{
public:
    Enemy(float x, float y, float speed, Color color); // Updated constructor
    void Update();
    void Draw() const;
    bool IsOffScreen(int screenHeight) const;
    Rectangle GetBounds() const;
    void Shoot();
    const std::vector<Projectile> &GetProjectiles() const;
    std::vector<Projectile> &GetProjectiles();
    void TakeDamage();
    bool IsDestroyed() const;

private:
    float x;
    float y;
    float speed;
    Color color;
    std::vector<Projectile> projectiles;
    float shootCooldown;       // Time between shots
    float timeSinceLastShot;   // Time since the last shot
    float movementPatternTime; // Time for movement pattern
    EnemyState state;          // Current state of the enemy
    int health;                // Enemy health, requires multiple hits

    void UpdateMovementPattern();
    void UpdateState();
};

#endif // ENEMY_H
