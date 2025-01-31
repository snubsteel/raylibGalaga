#include <raylib.h>
#include "Player.h"
#include "Star.h"
#include "Enemy.h"
#include <vector>
#include <iostream>

void SpawnEnemies(std::vector<Enemy>& enemies, int level, int wave) {
    int numEnemies = 5 + (level - 1) * 2 + wave; // Increase the number of enemies with each level and wave
    for (int i = 0; i < numEnemies; ++i) {
        enemies.emplace_back(GetRandomValue(0, GetScreenWidth()), GetRandomValue(-1000, -100), 2.0f);
    }
    std::cout << "Spawned " << numEnemies << " enemies for level " << level << ", wave " << wave << "." << std::endl;
}

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1920, 1080, "raylibGalaga");
    InitAudioDevice(); // Initialize audio device
    SetTargetFPS(60);

    // Load textures
    Texture2D playerTexture = LoadTexture("C:\\softwareProjects\\raylibGalaga\\res\\player_sprite.jpg");

    // Load music
    Music levelStart = LoadMusicStream("C:\\softwareProjects\\raylibGalaga\\res\\level_start.mp3"); 

    Player player(0.5f, 0.9f, playerTexture.width, playerTexture.height);

    // Create stars
    const int numStars = 100;
    std::vector<Star> stars;
    for (int i = 0; i < numStars; ++i) {
        stars.emplace_back(GetScreenWidth(), GetScreenHeight());
    }

    // Create enemies
    std::vector<Enemy> enemies;
    std::vector<Projectile> enemyProjectiles;

    int level = 1;
    int wave = 1;
    int score = 0;
    bool gameOver = false;
    bool levelStartMusicPlayed = false; // Flag to track if level start music has been played

    SpawnEnemies(enemies, level, wave);

    // GAME LOOP
    while (!WindowShouldClose())
    {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        // Play music at the start of the LEVEL
        if (!levelStartMusicPlayed) { // Play only at the start of a new level
            PlayMusicStream(levelStart);
            levelStartMusicPlayed = true;
        }

        UpdateMusicStream(levelStart);

        // Stop music after it plays once
        if (GetMusicTimePlayed(levelStart) >= GetMusicTimeLength(levelStart)) {
            StopMusicStream(levelStart); // Stop the music explicitly
        }

        // 1. EVENT HANDLING
        if (IsKeyPressed(KEY_F11))
        {
            ToggleFullscreen();
        }

        if (gameOver) {
            if (IsKeyPressed(KEY_ENTER)) {
                // Restart the game
                player = Player(0.5f, 0.9f, playerTexture.width, playerTexture.height);
                enemies.clear();
                enemyProjectiles.clear();
                level = 1; // Reset level
                wave = 1; // Reset wave
                SpawnEnemies(enemies, level, wave);
                levelStartMusicPlayed = false; // Reset the flag for the next level
                score = 0;
                gameOver = false;
            }

            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 40) / 2, screenHeight / 2 - 20, 40, RED);
            DrawText("Press ENTER to restart", screenWidth / 2 - MeasureText("Press ENTER to restart", 20) / 2, screenHeight / 2 + 20, 20, WHITE);
            EndDrawing();
            continue;
        } else {
            // 2. UPDATING POSITIONS
            player.Update(screenWidth, screenHeight);

            for (auto& star : stars) {
                star.Update(screenHeight);
            }

            for (auto& enemy : enemies) {
                enemy.Update();
            }

            if (!player.IsExploding()) {
                // Check for collisions between projectiles and enemies
                auto& projectiles = player.GetProjectiles();
                for (auto it = projectiles.begin(); it != projectiles.end(); ) {
                    bool hit = false;
                    for (auto et = enemies.begin(); et != enemies.end(); ) {
                        if (CheckCollisionRecs(it->GetBounds(), et->GetBounds())) {
                            std::vector<Projectile> enemyProj = et->GetProjectiles();
                            enemyProjectiles.insert(enemyProjectiles.end(), enemyProj.begin(), enemyProj.end());
                            it = projectiles.erase(it);
                            et = enemies.erase(et);
                            hit = true;
                            score += 100; // Increase score for each enemy hit
                            break;
                        } else {
                            ++et;
                        }
                    }
                    if (!hit) {
                        ++it;
                    }
                }

                // Check for collisions between player and enemies
                for (const auto& enemy : enemies) {
                    if (CheckCollisionRecs({static_cast<float>(player.GetX() - player.GetWidth() / 2), static_cast<float>(player.GetY() - player.GetHeight() / 2), static_cast<float>(player.GetWidth()), static_cast<float>(player.GetHeight())}, enemy.GetBounds())) {
                        player.LoseLife();
                        if (player.GetLives() <= 0) {
                            gameOver = true;
                            break;
                        }
                    }
                }

                // Check for collisions between player and enemy projectiles
                for (auto it = enemyProjectiles.begin(); it != enemyProjectiles.end(); ) {
                    if (CheckCollisionRecs(it->GetBounds(), {static_cast<float>(player.GetX() - player.GetWidth() / 2), static_cast<float>(player.GetY() - player.GetHeight() / 2), static_cast<float>(player.GetWidth()), static_cast<float>(player.GetHeight())})) {
                        it = enemyProjectiles.erase(it);
                        player.LoseLife();
                        if (player.GetLives() <= 0) {
                            gameOver = true;
                            break;
                        }
                    } else {
                        ++it;
                    }
                }

                // Update enemy projectiles even if the enemy is destroyed
                for (auto it = enemyProjectiles.begin(); it != enemyProjectiles.end(); ) {
                    it->Update();
                    if (it->IsOffScreen(screenHeight)) {
                        it = enemyProjectiles.erase(it);
                    } else {
                        ++it;
                    }
                }

                // Check if all enemies are off-screen
                bool allEnemiesOffScreen = true;
                for (const auto& enemy : enemies) {
                    if (!enemy.IsOffScreen(screenHeight)) {
                        allEnemiesOffScreen = false;
                        break;
                    }
                }

                if (allEnemiesOffScreen && !gameOver) {
                    wave++;
                    if (wave > level + 2) { // Example: level 1 has 3 waves, level 2 has 4 waves, etc.
                        level++;
                        wave = 1;
                        levelStartMusicPlayed = false; // Reset the flag for the next level
                    }
                    SpawnEnemies(enemies, level, wave);
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

            // Draw enemy projectiles
            for (const auto& projectile : enemyProjectiles) {
                projectile.Draw();
            }

            player.Draw(playerTexture);

            // Draw score, lives, and level
            DrawText(TextFormat("Score: %04i", score), 10, 10, 20, WHITE);
            DrawText(TextFormat("Lives: %i", player.GetLives()), 10, 40, 20, WHITE);
            DrawText(TextFormat("Level: %i", level), 10, 70, 20, WHITE);

            EndDrawing();
        }
    }

    UnloadTexture(playerTexture);
    UnloadMusicStream(levelStart); // Unload level start music
    CloseAudioDevice(); // Close audio device
    CloseWindow();
    return 0;
}