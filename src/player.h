#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include <vector>
#include "Projectile.h"

class Player {
public:
    Player(float posXPercent, float posYPercent, int width, int height);
    void Update(int screenWidth, int screenHeight);
    void Draw() const;
    std::vector<Projectile>& GetProjectiles();

private:
    float posXPercent;
    float posYPercent;
    int width;
    int height;
    int x;
    int y;
    float movementSpeed;
    std::vector<Projectile> projectiles;
    float shootCooldown; // Time between shots
    float timeSinceLastShot; // Time since the last shot

    void Shoot();
};

#endif // PLAYER_H