#include "Star.h"

Star::Star(int screenWidth, int screenHeight) {
    x = static_cast<float>(GetRandomValue(0, screenWidth));
    y = static_cast<float>(GetRandomValue(0, screenHeight));
    speed = static_cast<float>(GetRandomValue(1, 5));
    color = (Color){ 255, 255, 255, static_cast<unsigned char>(GetRandomValue(100, 255)) };
}

void Star::Update(int screenHeight) {
    y += speed;
    if (y > screenHeight) {
        y = 0;
        x = static_cast<float>(GetRandomValue(0, GetScreenWidth())); 
        speed = static_cast<float>(GetRandomValue(1, 5));
        color.a = static_cast<unsigned char>(GetRandomValue(100, 255));
    }
}

void Star::Draw() const {
    DrawPixel(static_cast<int>(x), static_cast<int>(y), color);
}