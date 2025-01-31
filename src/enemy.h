#ifndef ENEMY_H
#define ENEMY_H

#include <raylib.h>
#include <vector>
#include "Projectile.h"

enum EnemyState {
    ENTERING,
    FORMATION,
    ATTACKING
};

class Enemy {
public:
    Enemy(float x, float y, float speed);
    void Update();
    void Draw() const;
    bool IsOffScreen(int screenHeight) const;
    Rectangle GetBounds() const;
    void Shoot();
    const std::vector<Projectile>& GetProjectiles() const;
    std::vector<Projectile>& GetProjectiles();

private:
    float x;
    float y;
    float speed;
    Color color;
    std::vector<Projectile> projectiles;
    float shootCooldown; // Time between shots
    float timeSinceLastShot; // Time since the last shot
    float movementPatternTime; // Time for movement pattern
    EnemyState state; // Current state of the enemy

    void UpdateMovementPattern();
    void UpdateState();
};

#endif // ENEMY_H