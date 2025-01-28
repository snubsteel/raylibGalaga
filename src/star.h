#ifndef STAR_H
#define STAR_H

#include <raylib.h>

class Star {
public:
    Star(int screenWidth, int screenHeight);
    void Update(int screenHeight);
    void Draw() const;

private:
    float x;
    float y;
    float speed;
    Color color;
};

#endif // STAR_H