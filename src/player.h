#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include <vector>
#include "Projectile.h"

class Player {
public:
    Player(float posXPercent, float posYPercent, int width, int height);
    void Update(int screenWidth, int screenHeight);
    void Draw(Texture2D playerTexture) const;
    std::vector<Projectile>& GetProjectiles();
    int GetLives() const;
    void LoseLife();
    void Respawn();
    int GetX() const;
    int GetY() const;
    int GetWidth() const;
    int GetHeight() const;
    bool IsExploding() const;

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
    int lives; // Player lives
    bool exploding; // Explosion state
    float explosionTime; // Time since explosion
    bool shootFromLeft; // Flag to alternate shooting sides

    void Shoot();
};

#endif // PLAYER_H