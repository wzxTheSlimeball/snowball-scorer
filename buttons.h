#ifndef BUTTONS_H
#define BUTTONS_H

#include "raylib.h"
#include <string>
#include <vector>
#include <memory>
void updateMousePos();
class Button{
    public:
    Rectangle rect;
    std::vector<std::unique_ptr<Texture2D>> textures;
    Button(Rectangle &rect,Texture2D &texture);
    Button(Rectangle &rect,std::string file);
    ~Button();
    void addTexture(Texture2D &texture);
    void addTexture(std::string file);
    bool isHovered();
    bool isLeftClicked();
    bool isRightClicked();
    void draw(int);
};

#endif