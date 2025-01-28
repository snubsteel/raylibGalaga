#include <raylib.h>
#include "Player.h"
#include "Star.h"
#include "Enemy.h"
#include <vector>

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1920, 1080, "raylibGalaga");
    SetTargetFPS(60);

    Player player(0.5f, 0.9f, 100, 100);

    // Create stars
    const int numStars = 100;
    std::vector<Star> stars;
    for (int i = 0; i < numStars; ++i) {
        stars.emplace_back(GetScreenWidth(), GetScreenHeight());
    }

    // Create enemies
    std::vector<Enemy> enemies;
    const int numEnemies = 10;
    for (int i = 0; i < numEnemies; ++i) {
        enemies.emplace_back(GetRandomValue(0, GetScreenWidth()), GetRandomValue(-1000, -100), 2.0f);
    }

    // GAME LOOP
    while (!WindowShouldClose())
    {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        // 1. EVENT HANDLING
        if (IsKeyPressed(KEY_F11))
        {
            ToggleFullscreen();
        }

        // 2. UPDATING POSITIONS
        player.Update(screenWidth, screenHeight);

        for (auto& star : stars) {
            star.Update(screenHeight);
        }

        for (auto& enemy : enemies) {
            enemy.Update();
        }

        // Check for collisions between projectiles and enemies
        auto& projectiles = player.GetProjectiles();
        for (auto it = projectiles.begin(); it != projectiles.end(); ) {
            bool hit = false;
            for (auto et = enemies.begin(); et != enemies.end(); ) {
                if (CheckCollisionRecs(it->GetBounds(), et->GetBounds())) {
                    it = projectiles.erase(it);
                    et = enemies.erase(et);
                    hit = true;
                    break;
                } else {
                    ++et;
                }
            }
            if (!hit) {
                ++it;
            }
        }

        // 3. DRAWING
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw stars
        for (const auto& star : stars) {
            star.Draw();
        }

        // Draw enemies
        for (const auto& enemy : enemies) {
            enemy.Draw();
        }

        player.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}