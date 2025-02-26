#include <raylib.h>
#include <cmath> // For fminf() and fmaxf()
#include "Player.h"
#include "Star.h"
#include "Enemy.h"
#include <vector>
#include <iostream>
#include <string>

// Game state enum
enum GameState
{
    MENU,
    PLAYING,
    PAUSED,
    SETTINGS,
    GAME_OVER,
    EXIT_CONFIRMATION // New state for exit confirmation
};

// Constants
const int VIRTUAL_WIDTH = 1920;
const int VIRTUAL_HEIGHT = 1080;
const int MENU_BUTTON_WIDTH = 250;
const int MENU_BUTTON_HEIGHT = 50;

// Function prototypes
void SpawnEnemies(std::vector<Enemy> &enemies, int level, int wave);
void HandleGameplay(Player &player, std::vector<Enemy> &enemies, std::vector<Projectile> &enemyProjectiles,
                    std::vector<Star> &stars, int &score, int &level, int &wave, bool &levelStartMusicPlayed,
                    Music &levelStart, GameState &currentState, int screenWidth, int screenHeight,
                    Texture2D &playerTexture); // Added playerTexture parameter
void DrawMenu(Vector2 mousePoint, GameState &currentState, std::vector<Star> &stars, int screenWidth, int screenHeight);
void DrawSettings(Vector2 mousePoint, GameState &currentState, std::vector<Star> &stars,
                  int &resolutionIndex, std::vector<std::pair<int, int>> &resolutionOptions,
                  bool &isFullscreen, bool &isBorderless, int screenWidth, int screenHeight,
                  float &masterVolume, GameState &previousState); // Added masterVolume and previousState parameters
void DrawPauseMenu(Vector2 mousePoint, GameState &currentState, int screenWidth, int screenHeight);
void DrawGameOver(int score, GameState &currentState, Player &player, std::vector<Enemy> &enemies,
                  std::vector<Projectile> &enemyProjectiles, int &level, int &wave, bool &levelStartMusicPlayed);
void DrawExitConfirmation(Vector2 mousePoint, GameState &currentState, GameState &previousState, int screenWidth, int screenHeight);

int main()
{
    // Configure window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(VIRTUAL_WIDTH, VIRTUAL_HEIGHT, "Space Shooter");

    // Create render texture for fixed resolution
    RenderTexture2D target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

    // Disable default ESC key exiting behavior
    SetExitKey(KEY_NULL);

    // Initialize audio
    InitAudioDevice();
    SetTargetFPS(60);

    // Load textures - use relative paths
    Texture2D playerTexture = LoadTexture("res/player_sprite.jpg");
    if (playerTexture.id == 0)
    {
        std::cout << "Failed to load player texture. Check file path." << std::endl;
        // Fallback: create a blank texture
        Image img = GenImageColor(64, 64, YELLOW);
        playerTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    // Load music - use relative paths
    Music levelStart = LoadMusicStream("res/level_start.mp3");
    levelStart.looping = false;
    SetMusicVolume(levelStart, 0.1f);

    // Player initialization
    Player player(0.5f, 0.95f, playerTexture.width, playerTexture.height);

    // Create stars
    const int numStars = 100;
    std::vector<Star> stars;
    for (int i = 0; i < numStars; ++i)
    {
        stars.emplace_back(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    }

    // Create enemies and projectiles
    std::vector<Enemy> enemies;
    std::vector<Projectile> enemyProjectiles;

    // Game state variables
    int level = 1;
    int wave = 1;
    int score = 0;
    bool levelStartMusicPlayed = false;
    GameState currentState = MENU;
    GameState previousState = MENU; // New variable to track previous state

    // Settings variables
    int resolutionIndex = 0;
    std::vector<std::pair<int, int>> resolutionOptions = {{1280, 720}, {1920, 1080}, {1600, 900}};
    bool isFullscreen = false;
    bool isBorderless = false;
    float masterVolume = 1.0f; // Master volume (0.0f - 1.0f)

    // Initialize first wave of enemies
    SpawnEnemies(enemies, level, wave);

    // GAME LOOP
    while (!WindowShouldClose())
    {
        // Calculate window dimensions and scaling
        int windowWidth = GetScreenWidth();
        int windowHeight = GetScreenHeight();

        // Use virtual resolution for game logic
        int screenWidth = VIRTUAL_WIDTH;
        int screenHeight = VIRTUAL_HEIGHT;

        // Compute scale factor and offsets to maintain aspect ratio
        float scaleX = (float)windowWidth / VIRTUAL_WIDTH;
        float scaleY = (float)windowHeight / VIRTUAL_HEIGHT;
        float scale = fminf(scaleX, scaleY);
        float offsetX = (windowWidth - VIRTUAL_WIDTH * scale) / 2;
        float offsetY = (windowHeight - VIRTUAL_HEIGHT * scale) / 2;
        offsetX = fmaxf(offsetX, 0);

        // Map mouse coordinates to virtual resolution
        Vector2 rawMousePos = GetMousePosition();
        Vector2 mousePoint = {(rawMousePos.x - offsetX) / scale, (rawMousePos.y - offsetY) / scale};

        // Global ESC key handling - modified to go to settings instead of closing
        if (IsKeyPressed(KEY_ESCAPE))
        {
            if (currentState == PLAYING)
            {
                currentState = PAUSED;
            }
            else if (currentState == MENU || currentState == PAUSED || currentState == GAME_OVER)
            {
                previousState = currentState;
                currentState = SETTINGS;
            }
            else if (currentState == SETTINGS)
            {
                currentState = previousState;
            }
            else if (currentState == EXIT_CONFIRMATION)
            {
                currentState = SETTINGS;
            }
        }

        // Toggle fullscreen with F11
        if (IsKeyPressed(KEY_F11))
        {
            ToggleFullscreen();
            isFullscreen = !isFullscreen;
        }

        // Apply master volume
        SetMasterVolume(masterVolume);

        // Begin rendering to fixed resolution texture
        BeginTextureMode(target);
        ClearBackground(BLACK);

        // Handle different game states
        switch (currentState)
        {
        case MENU:
            DrawMenu(mousePoint, currentState, stars, screenWidth, screenHeight);
            break;

        case SETTINGS:
            DrawSettings(mousePoint, currentState, stars, resolutionIndex,
                         resolutionOptions, isFullscreen, isBorderless,
                         screenWidth, screenHeight, masterVolume, previousState);
            break;

        case EXIT_CONFIRMATION:
            DrawExitConfirmation(mousePoint, currentState, previousState, screenWidth, screenHeight);
            break;

        case PAUSED:
            // Draw the gameplay scene first (frozen)
            for (const auto &star : stars)
                star.Draw();
            for (const auto &enemy : enemies)
                enemy.Draw();
            for (const auto &projectile : enemyProjectiles)
                projectile.Draw();
            player.Draw(playerTexture);

            DrawPauseMenu(mousePoint, currentState, screenWidth, screenHeight);
            break;

        case GAME_OVER:
            // Draw stars in background
            for (auto &star : stars)
            {
                star.Update(screenHeight);
                star.Draw();
            }

            DrawGameOver(score, currentState, player, enemies, enemyProjectiles,
                         level, wave, levelStartMusicPlayed);
            break;

        case PLAYING:
            HandleGameplay(player, enemies, enemyProjectiles, stars, score, level,
                           wave, levelStartMusicPlayed, levelStart, currentState,
                           screenWidth, screenHeight, playerTexture); // Pass playerTexture
            break;
        }

        EndTextureMode();

        // Draw the render texture to the window
        BeginDrawing();
        ClearBackground(BLACK);
        Rectangle srcRec = {0, 0, (float)target.texture.width, -(float)target.texture.height};
        Rectangle destRec = {offsetX, offsetY, VIRTUAL_WIDTH * scale, VIRTUAL_HEIGHT * scale};
        Vector2 origin = {0, 0};
        DrawTexturePro(target.texture, srcRec, destRec, origin, 0.0f, WHITE);
        EndDrawing();

        // Update music
        UpdateMusicStream(levelStart);
    }

    // Cleanup resources
    UnloadRenderTexture(target);
    UnloadTexture(playerTexture);
    UnloadMusicStream(levelStart);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

void SpawnEnemies(std::vector<Enemy> &enemies, int level, int wave)
{
    // Increase number and difficulty of enemies based on level and wave
    int baseEnemies = 5;
    int numEnemies = baseEnemies + (level - 1) * 2 + wave;

    // Cap max enemies to prevent overwhelming the screen
    numEnemies = fminf(numEnemies, 20);

    // Increase enemy speed with levels
    float baseSpeed = 1.5f;
    float speedMultiplier = 1.0f + (level * 0.1f);
    float enemySpeed = baseSpeed * speedMultiplier;

    // Add formation patterns based on level
    int columns = 5;

    // Create enemies in a grid formation
    for (int i = 0; i < numEnemies; ++i)
    {
        int row = i / columns;
        int col = i % columns;
        float xPos = (GetScreenWidth() / (columns + 1)) * (col + 1);
        float yPos = -100 - (row * 80); // Start above screen with spacing

        // Determine enemy color based on level
        Color enemyColor;
        if (level == 1)
        {
            enemyColor = (GetRandomValue(0, 1) == 0) ? RED : ORANGE;
        }
        else if (level == 2)
        {
            int colorChoice = GetRandomValue(0, 2);
            enemyColor = (colorChoice == 0) ? RED : (colorChoice == 1) ? ORANGE
                                                                       : PINK;
        }
        else
        {
            int colorChoice = GetRandomValue(0, 3);
            enemyColor = (colorChoice == 0) ? RED : (colorChoice == 1) ? ORANGE
                                                : (colorChoice == 2)   ? PINK
                                                                       : PURPLE;
        }

        enemies.emplace_back(xPos, yPos, enemySpeed, enemyColor);
    }

    std::cout << "Spawned " << numEnemies << " enemies for level " << level
              << ", wave " << wave << " with speed " << enemySpeed << std::endl;
}

void HandleGameplay(Player &player, std::vector<Enemy> &enemies, std::vector<Projectile> &enemyProjectiles,
                    std::vector<Star> &stars, int &score, int &level, int &wave, bool &levelStartMusicPlayed,
                    Music &levelStart, GameState &currentState, int screenWidth, int screenHeight,
                    Texture2D &playerTexture)
{ // Added playerTexture parameter

    // Play level start music if needed
    if (!levelStartMusicPlayed)
    {
        PlayMusicStream(levelStart);
        levelStartMusicPlayed = true;
    }

    // Stop music after it plays once
    if (GetMusicTimePlayed(levelStart) >= GetMusicTimeLength(levelStart))
    {
        StopMusicStream(levelStart);
    }

    // Update game objects
    player.Update(screenWidth, screenHeight);

    for (auto &star : stars)
    {
        star.Update(screenHeight);
    }

    for (auto &enemy : enemies)
    {
        enemy.Update();
    }

    if (!player.IsExploding())
    {
        // Check for collisions between player projectiles and enemies
        auto &playerProjectiles = player.GetProjectiles();
        for (auto it = playerProjectiles.begin(); it != playerProjectiles.end();)
        {
            bool hit = false;
            for (auto et = enemies.begin(); et != enemies.end();)
            {
                if (CheckCollisionRecs(it->GetBounds(), et->GetBounds()))
                {
                    // Handle enemy hit
                    et->TakeDamage();

                    if (et->IsDestroyed())
                    {
                        // Add enemy projectiles to global list before destroying enemy
                        std::vector<Projectile> enemyProj = et->GetProjectiles();
                        enemyProjectiles.insert(enemyProjectiles.end(), enemyProj.begin(), enemyProj.end());

                        // Remove the enemy and award points
                        et = enemies.erase(et);
                        score += 100; // Base score per enemy
                    }
                    else
                    {
                        ++et; // Enemy still alive, move to next
                    }

                    // Remove the projectile that hit
                    it = playerProjectiles.erase(it);
                    hit = true;
                    break;
                }
                else
                {
                    ++et; // No collision, check next enemy
                }
            }

            if (!hit)
            {
                ++it; // No hit, check next projectile
            }
        }

        // Check for collisions between player and enemies
        for (const auto &enemy : enemies)
        {
            if (CheckCollisionRecs(
                    {static_cast<float>(player.GetX() - player.GetWidth() / 2),
                     static_cast<float>(player.GetY() - player.GetHeight() / 2),
                     static_cast<float>(player.GetWidth()),
                     static_cast<float>(player.GetHeight())},
                    enemy.GetBounds()))
            {

                player.LoseLife();
                if (player.GetLives() <= 0)
                {
                    currentState = GAME_OVER;
                    break;
                }
            }
        }

        // Check for collisions between player and enemy projectiles
        for (auto it = enemyProjectiles.begin(); it != enemyProjectiles.end();)
        {
            if (CheckCollisionRecs(
                    it->GetBounds(),
                    {static_cast<float>(player.GetX() - player.GetWidth() / 2),
                     static_cast<float>(player.GetY() - player.GetHeight() / 2),
                     static_cast<float>(player.GetWidth()),
                     static_cast<float>(player.GetHeight())}))
            {

                it = enemyProjectiles.erase(it);
                player.LoseLife();
                if (player.GetLives() <= 0)
                {
                    currentState = GAME_OVER;
                    break;
                }
            }
            else
            {
                ++it;
            }
        }

        // Update enemy projectiles even if the enemy is destroyed
        for (auto it = enemyProjectiles.begin(); it != enemyProjectiles.end();)
        {
            it->Update();
            if (it->IsOffScreen(screenHeight))
            {
                it = enemyProjectiles.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // Check if all enemies are destroyed or off-screen
        bool allEnemiesGone = enemies.empty();
        if (!allEnemiesGone)
        {
            allEnemiesGone = true;
            for (const auto &enemy : enemies)
            {
                if (!enemy.IsOffScreen(screenHeight))
                {
                    allEnemiesGone = false;
                    break;
                }
            }
        }

        // Spawn next wave if all enemies are gone
        if (allEnemiesGone && !player.IsExploding())
        {
            wave++;
            if (wave > level + 2)
            {
                level++;
                wave = 1;
                levelStartMusicPlayed = false;
            }
            SpawnEnemies(enemies, level, wave);
        }
    }

    // Handle player respawn
    if (player.IsExploding() && player.GetExplosionTime() >= 2.0f)
    {
        bool enemiesCleared = true;
        for (const auto &enemy : enemies)
        {
            if (!enemy.IsOffScreen(screenHeight) &&
                CheckCollisionRecs(
                    {static_cast<float>(player.GetX() - player.GetWidth()),
                     static_cast<float>(player.GetY() - player.GetHeight()),
                     static_cast<float>(player.GetWidth() * 2),
                     static_cast<float>(player.GetHeight() * 2)},
                    enemy.GetBounds()))
            {
                enemiesCleared = false;
                break;
            }
        }
        if (enemiesCleared)
        {
            player.Respawn();
        }
    }

    // Draw everything
    for (const auto &star : stars)
    {
        star.Draw();
    }

    for (const auto &enemy : enemies)
    {
        enemy.Draw();
    }

    for (const auto &projectile : enemyProjectiles)
    {
        projectile.Draw();
    }

    player.Draw(playerTexture);

    // Draw HUD (score, lives, level, wave)
    DrawText(TextFormat("Score: %06i", score), 10, 10, 20, WHITE);
    DrawText(TextFormat("Lives: %i", player.GetLives()), 10, 40, 20, WHITE);
    DrawText(TextFormat("Level: %i", level), 10, 70, 20, WHITE);
    DrawText(TextFormat("Wave: %i/%i", wave, level + 2), 10, 100, 20, WHITE);
}

void DrawMenu(Vector2 mousePoint, GameState &currentState, std::vector<Star> &stars,
              int screenWidth, int screenHeight)
{

    // Update and draw stars in the background
    for (auto &star : stars)
    {
        star.Update(screenHeight);
        star.Draw();
    }

    // Draw title
    DrawText("SPACE SHOOTER", screenWidth / 2 - MeasureText("SPACE SHOOTER", 60) / 2, screenHeight / 2 - 200, 60, WHITE);

    // Define buttons
    int buttonWidth = MENU_BUTTON_WIDTH;
    int buttonHeight = MENU_BUTTON_HEIGHT;
    Rectangle startBtn = {(float)(screenWidth / 2 - buttonWidth / 2), (float)(screenHeight / 2 - 80), (float)buttonWidth, (float)buttonHeight};
    Rectangle settingsBtn = {(float)(screenWidth / 2 - buttonWidth / 2), (float)(screenHeight / 2), (float)buttonWidth, (float)buttonHeight};
    Rectangle exitBtn = {(float)(screenWidth / 2 - buttonWidth / 2), (float)(screenHeight / 2 + 80), (float)buttonWidth, (float)buttonHeight};

    // Draw Start button
    Color btnColor = CheckCollisionPointRec(mousePoint, startBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(startBtn, btnColor);
    int textWidth = MeasureText("Start", 20);
    DrawText("Start", startBtn.x + buttonWidth / 2 - textWidth / 2, startBtn.y + buttonHeight / 2 - 10, 20, BLACK);

    // Draw Settings button
    btnColor = CheckCollisionPointRec(mousePoint, settingsBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(settingsBtn, btnColor);
    textWidth = MeasureText("Settings", 20);
    DrawText("Settings", settingsBtn.x + buttonWidth / 2 - textWidth / 2, settingsBtn.y + buttonHeight / 2 - 10, 20, BLACK);

    // Draw Exit button
    btnColor = CheckCollisionPointRec(mousePoint, exitBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(exitBtn, btnColor);
    textWidth = MeasureText("Exit", 20);
    DrawText("Exit", exitBtn.x + buttonWidth / 2 - textWidth / 2, exitBtn.y + buttonHeight / 2 - 10, 20, BLACK);

    // Button click handling
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mousePoint, startBtn))
        {
            currentState = PLAYING;
        }
        if (CheckCollisionPointRec(mousePoint, settingsBtn))
        {
            currentState = SETTINGS;
        }
        if (CheckCollisionPointRec(mousePoint, exitBtn))
        {
            CloseWindow();
        }
    }
}

void DrawSettings(Vector2 mousePoint, GameState &currentState, std::vector<Star> &stars,
                  int &resolutionIndex, std::vector<std::pair<int, int>> &resolutionOptions,
                  bool &isFullscreen, bool &isBorderless, int screenWidth, int screenHeight,
                  float &masterVolume, GameState &previousState)
{
    // Update and draw stars in the background
    for (auto &star : stars)
    {
        star.Update(screenHeight);
        star.Draw();
    }

    // Define buttons
    int btnWidth = MENU_BUTTON_WIDTH;
    int btnHeight = MENU_BUTTON_HEIGHT;
    Rectangle resolutionBtn = {(float)(screenWidth / 2 - btnWidth / 2), (float)(screenHeight / 2 - 190), (float)btnWidth, (float)btnHeight};
    Rectangle fullscreenBtn = {(float)(screenWidth / 2 - btnWidth / 2), (float)(screenHeight / 2 - 120), (float)btnWidth, (float)btnHeight};
    Rectangle borderlessBtn = {(float)(screenWidth / 2 - btnWidth / 2), (float)(screenHeight / 2 - 50), (float)btnWidth, (float)btnHeight};
    Rectangle volumeDecBtn = {(float)(screenWidth / 2 - btnWidth / 2), (float)(screenHeight / 2 + 20), (float)btnWidth / 3, (float)btnHeight};
    Rectangle volumeIncBtn = {(float)(screenWidth / 2 + btnWidth / 6), (float)(screenHeight / 2 + 20), (float)btnWidth / 3, (float)btnHeight};
    Rectangle backBtn = {(float)(screenWidth / 2 - btnWidth / 2), (float)(screenHeight / 2 + 90), (float)btnWidth, (float)btnHeight};
    Rectangle exitBtn = {(float)(screenWidth / 2 - btnWidth / 2), (float)(screenHeight / 2 + 160), (float)btnWidth, (float)btnHeight};

    // Draw Settings title
    DrawText("Settings", screenWidth / 2 - MeasureText("Settings", 40) / 2, screenHeight / 2 - 270, 40, WHITE);

    // Draw Resolution button
    std::string resText = "Resolution: " + std::to_string(resolutionOptions[resolutionIndex].first) +
                          "x" + std::to_string(resolutionOptions[resolutionIndex].second);
    Color btnColor = CheckCollisionPointRec(mousePoint, resolutionBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(resolutionBtn, btnColor);
    int textWidth = MeasureText(resText.c_str(), 20);
    DrawText(resText.c_str(), resolutionBtn.x + btnWidth / 2 - textWidth / 2, resolutionBtn.y + btnHeight / 2 - 10, 20, BLACK);

    // Draw Fullscreen button
    std::string fullText = "Fullscreen: ";
    fullText += isFullscreen ? "On" : "Off";
    btnColor = CheckCollisionPointRec(mousePoint, fullscreenBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(fullscreenBtn, btnColor);
    textWidth = MeasureText(fullText.c_str(), 20);
    DrawText(fullText.c_str(), fullscreenBtn.x + btnWidth / 2 - textWidth / 2, fullscreenBtn.y + btnHeight / 2 - 10, 20, BLACK);

    // Draw Borderless button
    std::string borderText = "Borderless: ";
    borderText += isBorderless ? "On" : "Off";
    btnColor = CheckCollisionPointRec(mousePoint, borderlessBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(borderlessBtn, btnColor);
    textWidth = MeasureText(borderText.c_str(), 20);
    DrawText(borderText.c_str(), borderlessBtn.x + btnWidth / 2 - textWidth / 2, borderlessBtn.y + btnHeight / 2 - 10, 20, BLACK);

    // Draw Volume control
    DrawText("Volume:", screenWidth / 2 - btnWidth / 2, screenHeight / 2 + 0, 20, WHITE);

    // Volume down button
    btnColor = CheckCollisionPointRec(mousePoint, volumeDecBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(volumeDecBtn, btnColor);
    DrawText("-", volumeDecBtn.x + volumeDecBtn.width / 2 - MeasureText("-", 20) / 2,
             volumeDecBtn.y + btnHeight / 2 - 10, 20, BLACK);

    // Volume display
    int volumePercentage = static_cast<int>(masterVolume * 100);
    std::string volumeText = std::to_string(volumePercentage) + "%";
    DrawText(volumeText.c_str(),
             screenWidth / 2 - MeasureText(volumeText.c_str(), 20) / 2,
             screenHeight / 2 + 20, 20, WHITE);

    // Volume up button
    btnColor = CheckCollisionPointRec(mousePoint, volumeIncBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(volumeIncBtn, btnColor);
    DrawText("+", volumeIncBtn.x + volumeIncBtn.width / 2 - MeasureText("+", 20) / 2,
             volumeIncBtn.y + btnHeight / 2 - 10, 20, BLACK);

    // Draw Back button
    btnColor = CheckCollisionPointRec(mousePoint, backBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(backBtn, btnColor);
    textWidth = MeasureText("Back", 20);
    DrawText("Back", backBtn.x + btnWidth / 2 - textWidth / 2, backBtn.y + btnHeight / 2 - 10, 20, BLACK);

    // Draw Exit Game button
    btnColor = CheckCollisionPointRec(mousePoint, exitBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(exitBtn, btnColor);
    textWidth = MeasureText("Exit Game", 20);
    DrawText("Exit Game", exitBtn.x + btnWidth / 2 - textWidth / 2, exitBtn.y + btnHeight / 2 - 10, 20, BLACK);

    // Button click handling
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
            if (!isBorderless)
            {
                SetWindowState(FLAG_WINDOW_UNDECORATED);
                isBorderless = true;
            }
            else
            {
                ClearWindowState(FLAG_WINDOW_UNDECORATED);
                isBorderless = false;
            }
        }
        else if (CheckCollisionPointRec(mousePoint, volumeDecBtn))
        {
            masterVolume = fmaxf(masterVolume - 0.1f, 0.0f);
        }
        else if (CheckCollisionPointRec(mousePoint, volumeIncBtn))
        {
            masterVolume = fminf(masterVolume + 0.1f, 1.0f);
        }
        else if (CheckCollisionPointRec(mousePoint, backBtn))
        {
            currentState = previousState;
        }
        else if (CheckCollisionPointRec(mousePoint, exitBtn))
        {
            currentState = EXIT_CONFIRMATION;
        }
    }
}

void DrawExitConfirmation(Vector2 mousePoint, GameState &currentState, GameState &previousState, int screenWidth, int screenHeight)
{
    // Draw semi-transparent overlay
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));

    // Draw confirmation dialog box
    int dialogWidth = 400;
    int dialogHeight = 200;
    int dialogX = screenWidth / 2 - dialogWidth / 2;
    int dialogY = screenHeight / 2 - dialogHeight / 2;

    DrawRectangle(dialogX, dialogY, dialogWidth, dialogHeight, DARKGRAY);
    DrawRectangleLines(dialogX, dialogY, dialogWidth, dialogHeight, WHITE);

    // Draw confirmation text
    const char *confirmText = "Are you sure you want to exit?";
    DrawText(confirmText,
             screenWidth / 2 - MeasureText(confirmText, 24) / 2,
             dialogY + 40, 24, WHITE);

    // Define buttons
    int btnWidth = 120;
    int btnHeight = 40;
    Rectangle yesBtn = {(float)(dialogX + dialogWidth / 4 - btnWidth / 2),
                        (float)(dialogY + dialogHeight - 60),
                        (float)btnWidth, (float)btnHeight};

    Rectangle noBtn = {(float)(dialogX + dialogWidth * 3 / 4 - btnWidth / 2),
                       (float)(dialogY + dialogHeight - 60),
                       (float)btnWidth, (float)btnHeight};

    // Draw Yes button
    Color btnColor = CheckCollisionPointRec(mousePoint, yesBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(yesBtn, btnColor);
    DrawText("Yes",
             yesBtn.x + btnWidth / 2 - MeasureText("Yes", 20) / 2,
             yesBtn.y + btnHeight / 2 - 10, 20, BLACK);

    // Draw No button
    btnColor = CheckCollisionPointRec(mousePoint, noBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(noBtn, btnColor);
    DrawText("No",
             noBtn.x + btnWidth / 2 - MeasureText("No", 20) / 2,
             noBtn.y + btnHeight / 2 - 10, 20, BLACK);

    // Handle button clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mousePoint, yesBtn))
        {
            CloseWindow();
        }
        else if (CheckCollisionPointRec(mousePoint, noBtn))
        {
            currentState = SETTINGS;
        }
    }
}

void DrawPauseMenu(Vector2 mousePoint, GameState &currentState, int screenWidth, int screenHeight)
{
    // Draw semi-transparent overlay
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));

    // Draw pause title
    DrawText("PAUSED", screenWidth / 2 - MeasureText("PAUSED", 40) / 2, screenHeight / 2 - 150, 40, WHITE);

    // Define buttons
    int btnWidth = MENU_BUTTON_WIDTH;
    int btnHeight = MENU_BUTTON_HEIGHT;
    Rectangle resumeBtn = {(float)(screenWidth / 2 - btnWidth / 2), (float)(screenHeight / 2 - 80), (float)btnWidth, (float)btnHeight};
    Rectangle settingsBtn = {(float)(screenWidth / 2 - btnWidth / 2), (float)(screenHeight / 2), (float)btnWidth, (float)btnHeight};
    Rectangle exitBtn = {(float)(screenWidth / 2 - btnWidth / 2), (float)(screenHeight / 2 + 80), (float)btnWidth, (float)btnHeight};

    // Draw Resume button
    Color btnColor = CheckCollisionPointRec(mousePoint, resumeBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(resumeBtn, btnColor);
    int textWidth = MeasureText("Resume Game", 20);
    DrawText("Resume Game", resumeBtn.x + btnWidth / 2 - textWidth / 2, resumeBtn.y + btnHeight / 2 - 10, 20, BLACK);

    // Draw Settings button
    btnColor = CheckCollisionPointRec(mousePoint, settingsBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(settingsBtn, btnColor);
    textWidth = MeasureText("Settings", 20);
    DrawText("Settings", settingsBtn.x + btnWidth / 2 - textWidth / 2, settingsBtn.y + btnHeight / 2 - 10, 20, BLACK);

    // Draw Exit to Main Menu button
    btnColor = CheckCollisionPointRec(mousePoint, exitBtn) ? GRAY : LIGHTGRAY;
    DrawRectangleRec(exitBtn, btnColor);
    textWidth = MeasureText("Exit to Main Menu", 20);
    DrawText("Exit to Main Menu", exitBtn.x + btnWidth / 2 - textWidth / 2, exitBtn.y + btnHeight / 2 - 10, 20, BLACK);

    // Button click handling
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (CheckCollisionPointRec(mousePoint, resumeBtn))
            currentState = PLAYING;
        if (CheckCollisionPointRec(mousePoint, settingsBtn))
            currentState = SETTINGS;
        if (CheckCollisionPointRec(mousePoint, exitBtn))
            currentState = MENU;
    }
}

void DrawGameOver(int score, GameState &currentState, Player &player, std::vector<Enemy> &enemies,
                  std::vector<Projectile> &enemyProjectiles, int &level, int &wave, bool &levelStartMusicPlayed)
{

    // Draw game over screen
    DrawText("GAME OVER", VIRTUAL_WIDTH / 2 - MeasureText("GAME OVER", 60) / 2, VIRTUAL_HEIGHT / 2 - 100, 60, RED);

    // Draw final score
    std::string scoreText = "Final Score: " + std::to_string(score);
    DrawText(scoreText.c_str(), VIRTUAL_WIDTH / 2 - MeasureText(scoreText.c_str(), 30) / 2, VIRTUAL_HEIGHT / 2, 30, WHITE);

    // Draw restart instruction
    DrawText("Press ENTER to restart", VIRTUAL_WIDTH / 2 - MeasureText("Press ENTER to restart", 20) / 2, VIRTUAL_HEIGHT / 2 + 80, 20, WHITE);
    DrawText("Press ESC to quit", VIRTUAL_WIDTH / 2 - MeasureText("Press ESC to quit", 20) / 2, VIRTUAL_HEIGHT / 2 + 120, 20, WHITE);

    // Handle restart
    if (IsKeyPressed(KEY_ENTER))
    {
        // Reset game state
        player = Player(0.5f, 0.95f, 64, 64); // Use default size if texture was unavailable
        enemies.clear();
        enemyProjectiles.clear();
        level = 1;
        wave = 1;
        SpawnEnemies(enemies, level, wave);
        levelStartMusicPlayed = false;
        currentState = PLAYING;
    }
}
