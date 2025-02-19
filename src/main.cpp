#include <raylib.h>
#include <cmath> // Added to fix fminf() not declared issue
#include "Player.h"
#include "Star.h"
#include "Enemy.h"
#include <vector>
#include <iostream>

// Update GameState enum:
enum GameState { MENU, PLAYING, PAUSED, SETTINGS };

void SpawnEnemies(std::vector<Enemy>& enemies, int level, int wave) {
    int numEnemies = 5 + (level - 1) * 2 + wave; // Increase the number of enemies with each level and wave
    for (int i = 0; i < numEnemies; ++i) {
        enemies.emplace_back(GetRandomValue(0, GetScreenWidth()), GetRandomValue(-1200, -100), 2.0f); // Changed Y range: now spawns from -1200 instead of -1000
    }
    std::cout << "Spawned " << numEnemies << " enemies for level " << level << ", wave " << wave << "." << std::endl;
}



int main()
{
    // Change fixed virtual resolution to 1280x720
    const int virtualWidth = 1920;
    const int virtualHeight = 1080;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(virtualWidth, virtualHeight, "raylibGalaga");
    // Create RenderTexture2D target for fixed resolution rendering
    RenderTexture2D target = LoadRenderTexture(virtualWidth, virtualHeight);
    
    InitAudioDevice(); // Initialize audio device
    SetTargetFPS(60);

    // Load textures
    Texture2D playerTexture = LoadTexture("C:\\softwareProjects\\raylibGalaga\\res\\player_sprite.jpg");

    // Load music
    Music levelStart = LoadMusicStream("C:\\softwareProjects\\raylibGalaga\\res\\level_start.mp3");
    levelStart.looping = false; // Disable looping so the audio plays only once
    SetMusicVolume(levelStart, 0.01f); // Lower the volume of the startLevel audio

    // Lower player's starting position for more vertical space
    Player player(0.5f, 0.95f, playerTexture.width, playerTexture.height); // Changed Y from 0.9f to 0.95f

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

    // Settings variables
    int resolutionIndex = 0;
    std::vector<std::pair<int,int>> resolutionOptions = { {1280,720}, {1920,1080}, {1600,900} };
    bool isFullscreen = false;
    bool isBorderless = false;

    SpawnEnemies(enemies, level, wave);

    GameState currentState = MENU; // Start at the menu

    // GAME LOOP
    while (!WindowShouldClose())
    {
        int windowWidth = GetScreenWidth();
        int windowHeight = GetScreenHeight();

        // Use virtual resolution for game logic
        int screenWidth = virtualWidth;   // Declared for game logic
        int screenHeight = virtualHeight; // Declared for game logic

        // Compute scale factor and offsets to maintain aspect ratio
        float scaleX = (float)windowWidth / virtualWidth;
        float scaleY = (float)windowHeight / virtualHeight;
        float scale = fminf(scaleX, scaleY); // fminf from <cmath>
        float offsetX = (windowWidth - virtualWidth * scale) / 2;
        float offsetY = (windowHeight - virtualHeight * scale) / 2;
        offsetX = fmaxf(offsetX, 0);  // Prevent negative offsetX causing a right gap
        
        // Map raw mouse coordinates to the virtual resolution coordinate space
        Vector2 rawMousePos = GetMousePosition();
        Vector2 mousePoint = { (rawMousePos.x - offsetX) / scale, (rawMousePos.y - offsetY) / scale };

        // Modify ESC handling:
        if (IsKeyPressed(KEY_ESCAPE))
        {
            if (currentState == PLAYING)
            {
                currentState = PAUSED; // Pause game when in PLAYING
            }
            else if (currentState == MENU || currentState == PAUSED)
            {
                CloseWindow();  // Exit game when in MENU or PAUSED
            }
        }

        // Start rendering into the fixed resolution render texture
        BeginTextureMode(target);
            ClearBackground(BLACK);
            
            // --- MENU UI ---
            if (currentState == MENU)
            {
                int buttonWidth = 200, buttonHeight = 50;
                Rectangle startBtn    = { (float)(screenWidth/2 - buttonWidth/2), (float)(screenHeight/2 - 80), (float)buttonWidth, (float)buttonHeight };
                Rectangle settingsBtn = { (float)(screenWidth/2 - buttonWidth/2), (float)(screenHeight/2),      (float)buttonWidth, (float)buttonHeight };
                Rectangle exitBtn     = { (float)(screenWidth/2 - buttonWidth/2), (float)(screenHeight/2 + 80), (float)buttonWidth, (float)buttonHeight };
                
                // Update and draw stars in the background
                for (auto& star : stars) { 
                    star.Update(screenHeight); 
                    star.Draw(); 
                }
                
                // Draw Start button
                Color btnColor = CheckCollisionPointRec(mousePoint, startBtn) ? GRAY : LIGHTGRAY;
                DrawRectangleRec(startBtn, btnColor);
                int textWidth = MeasureText("Start", 20);
                DrawText("Start", startBtn.x + buttonWidth/2 - textWidth/2, startBtn.y + buttonHeight/2 - 10, 20, BLACK);
                
                // Draw Settings button
                btnColor = CheckCollisionPointRec(mousePoint, settingsBtn) ? GRAY : LIGHTGRAY;
                DrawRectangleRec(settingsBtn, btnColor);
                textWidth = MeasureText("Settings", 20);
                DrawText("Settings", settingsBtn.x + buttonWidth/2 - textWidth/2, settingsBtn.y + buttonHeight/2 - 10, 20, BLACK);
                
                // Draw Exit button
                btnColor = CheckCollisionPointRec(mousePoint, exitBtn) ? GRAY : LIGHTGRAY;
                DrawRectangleRec(exitBtn, btnColor);
                textWidth = MeasureText("Exit", 20);
                DrawText("Exit", exitBtn.x + buttonWidth/2 - textWidth/2, exitBtn.y + buttonHeight/2 - 10, 20, BLACK);
                
                // Button click handling
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (CheckCollisionPointRec(mousePoint, startBtn)) { 
                        currentState = PLAYING; // Transition to gameplay only when Start is pressed
                    }
                    if (CheckCollisionPointRec(mousePoint, settingsBtn)) { 
                        currentState = SETTINGS; 
                    }
                    if (CheckCollisionPointRec(mousePoint, exitBtn)) { 
                        break; 
                    }
                }
            }
            
            // --- SETTINGS UI ---
            if (currentState == SETTINGS)
            {
                int btnWidth = 250, btnHeight = 50;
                Rectangle resolutionBtn = { (float)(screenWidth/2 - btnWidth/2), (float)(screenHeight/2 - 120), (float)btnWidth, (float)btnHeight };
                Rectangle fullscreenBtn = { (float)(screenWidth/2 - btnWidth/2), (float)(screenHeight/2 - 50),  (float)btnWidth, (float)btnHeight };
                Rectangle borderlessBtn = { (float)(screenWidth/2 - btnWidth/2), (float)(screenHeight/2 + 20),  (float)btnWidth, (float)btnHeight };
                Rectangle backBtn       = { (float)(screenWidth/2 - btnWidth/2), (float)(screenHeight/2 + 90),  (float)btnWidth, (float)btnHeight };
                
                // Update and draw stars in the background
                for (auto& star : stars) { 
                    star.Update(screenHeight); 
                    star.Draw(); 
                }
                
                // Draw Settings title
                DrawText("Settings", screenWidth/2 - MeasureText("Settings", 30)/2, screenHeight/2 - 180, 30, WHITE);
                
                // Draw Resolution button
                std::string resText = "Resolution: " + std::to_string(resolutionOptions[resolutionIndex].first) + "x" + std::to_string(resolutionOptions[resolutionIndex].second);
                Color btnColor = CheckCollisionPointRec(mousePoint, resolutionBtn) ? GRAY : LIGHTGRAY;
                DrawRectangleRec(resolutionBtn, btnColor);
                int textWidth = MeasureText(resText.c_str(), 20);
                DrawText(resText.c_str(), resolutionBtn.x + btnWidth/2 - textWidth/2, resolutionBtn.y + btnHeight/2 - 10, 20, BLACK);
                
                // Draw Fullscreen button
                std::string fullText = "Fullscreen: ";
                fullText += isFullscreen ? "On" : "Off";
                btnColor = CheckCollisionPointRec(mousePoint, fullscreenBtn) ? GRAY : LIGHTGRAY;
                DrawRectangleRec(fullscreenBtn, btnColor);
                textWidth = MeasureText(fullText.c_str(), 20);
                DrawText(fullText.c_str(), fullscreenBtn.x + btnWidth/2 - textWidth/2, fullscreenBtn.y + btnHeight/2 - 10, 20, BLACK);
                
                // Draw Borderless button
                std::string borderText = "Borderless: ";
                borderText += isBorderless ? "On" : "Off";
                btnColor = CheckCollisionPointRec(mousePoint, borderlessBtn) ? GRAY : LIGHTGRAY;
                DrawRectangleRec(borderlessBtn, btnColor);
                textWidth = MeasureText(borderText.c_str(), 20);
                DrawText(borderText.c_str(), borderlessBtn.x + btnWidth/2 - textWidth/2, borderlessBtn.y + btnHeight/2 - 10, 20, BLACK);
                
                // Draw Back button
                btnColor = CheckCollisionPointRec(mousePoint, backBtn) ? GRAY : LIGHTGRAY;
                DrawRectangleRec(backBtn, btnColor);
                textWidth = MeasureText("Back", 20);
                DrawText("Back", backBtn.x + btnWidth/2 - textWidth/2, backBtn.y + btnHeight/2 - 10, 20, BLACK);
                
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    if (CheckCollisionPointRec(mousePoint, resolutionBtn))
                    {
                        resolutionIndex = (resolutionIndex + 1) % resolutionOptions.size();
                        SetWindowSize(resolutionOptions[resolutionIndex].first, resolutionOptions[resolutionIndex].second);
                    }
                    else if (CheckCollisionPointRec(mousePoint, fullscreenBtn))
                    {
                        ToggleFullscreen();
                        isFullscreen = !isFullscreen;
                    }
                    else if (CheckCollisionPointRec(mousePoint, borderlessBtn))
                    {
                        if (!isBorderless) {
                            SetWindowState(FLAG_WINDOW_UNDECORATED);
                            isBorderless = true;
                        } else {
                            ClearWindowState(FLAG_WINDOW_UNDECORATED);
                            isBorderless = false;
                        }
                    }
                    else if (CheckCollisionPointRec(mousePoint, backBtn))
                    {
                        currentState = MENU;
                    }
                }
            }
            else if (currentState == PAUSED)
            {
                // Draw semi-transparent overlay
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));

                // Define button dimensions:
                int btnWidth = 250, btnHeight = 50;
                // Three buttons: Resume, Settings, Exit to Main Menu
                Rectangle resumeBtn = { (float)(screenWidth/2 - btnWidth/2), (float)(screenHeight/2 - 80), (float)btnWidth, (float)btnHeight };
                Rectangle settingsBtn = { (float)(screenWidth/2 - btnWidth/2), (float)(screenHeight/2), (float)btnWidth, (float)btnHeight };
                Rectangle exitBtn = { (float)(screenWidth/2 - btnWidth/2), (float)(screenHeight/2 + 80), (float)btnWidth, (float)btnHeight };

                // Draw Resume button
                Color btnColor = CheckCollisionPointRec(mousePoint, resumeBtn) ? GRAY : LIGHTGRAY;
                DrawRectangleRec(resumeBtn, btnColor);
                int textWidth = MeasureText("Resume Game", 20);
                DrawText("Resume Game", resumeBtn.x + btnWidth/2 - textWidth/2, resumeBtn.y + btnHeight/2 - 10, 20, BLACK);

                // Draw Settings button
                btnColor = CheckCollisionPointRec(mousePoint, settingsBtn) ? GRAY : LIGHTGRAY;
                DrawRectangleRec(settingsBtn, btnColor);
                textWidth = MeasureText("Settings", 20);
                DrawText("Settings", settingsBtn.x + btnWidth/2 - textWidth/2, settingsBtn.y + btnHeight/2 - 10, 20, BLACK);

                // Draw Exit to Main Menu button
                btnColor = CheckCollisionPointRec(mousePoint, exitBtn) ? GRAY : LIGHTGRAY;
                DrawRectangleRec(exitBtn, btnColor);
                textWidth = MeasureText("Exit to Main Menu", 20);
                DrawText("Exit to Main Menu", exitBtn.x + btnWidth/2 - textWidth/2, exitBtn.y + btnHeight/2 - 10, 20, BLACK);

                // Button click handling for pause menu:
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    if (CheckCollisionPointRec(mousePoint, resumeBtn))
                        currentState = PLAYING;  // Resume game
                    if (CheckCollisionPointRec(mousePoint, settingsBtn))
                        currentState = SETTINGS;
                    if (CheckCollisionPointRec(mousePoint, exitBtn))
                        currentState = MENU;
                }
            }
            else if (currentState == PLAYING)
            {
                // --- GAMEPLAY ---
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

                    DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 40) / 2, screenHeight / 2 - 20, 40, RED);
                    DrawText("Press ENTER to restart", screenWidth / 2 - MeasureText("Press ENTER to restart", 20) / 2, screenHeight / 2 + 20, 20, WHITE);
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

                    // Check if player is exploding and ready to respawn:
                    if (player.IsExploding() && player.GetExplosionTime() >= 3.0f) {
                        bool enemiesCleared = true;
                        for (const auto& enemy : enemies) {
                            if (!enemy.IsOffScreen(screenHeight)) {
                                enemiesCleared = false;
                                break;
                            }
                        }
                        if (enemiesCleared) {
                            player.Respawn();
                        }
                    }
                    
                    // 3. DRAWING
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
                }
            }
        EndTextureMode();
    
        // Draw the render texture to the window without stretching:
        BeginDrawing();
            ClearBackground(BLACK);
            Rectangle srcRec = { 0, 0, (float)target.texture.width, - (float)target.texture.height };
            Rectangle destRec = { offsetX, offsetY, virtualWidth * scale, virtualHeight * scale };
            Vector2 origin = { 0, 0 };
            DrawTexturePro(target.texture, srcRec, destRec, origin, 0.0f, WHITE);
        EndDrawing();
        
        // ...existing audio and game state update logic...
    }

    UnloadRenderTexture(target);
    UnloadTexture(playerTexture);
    UnloadMusicStream(levelStart); // Unload level start music
    CloseAudioDevice(); // Close audio device
    CloseWindow();
    return 0;
}