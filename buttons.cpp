#include "buttons.h"
#include "raylib.h"
#include "safelog.h"
#include <string>
#include <memory>
Vector2 mousePos={0.0f,0.0f};
void updateMousePos(){
    mousePos=GetMousePosition();
}
Button::Button(Rectangle &rect,Texture2D &texture){
    this->rect=rect;
    auto texture_unique=std::make_unique<Texture2D>();
    *texture_unique=texture;
    this->textures.push_back(std::move(texture_unique));
}
Button::Button(Rectangle &rect,std::string file){
    this->rect=rect;
    auto texture_unique=std::make_unique<Texture2D>();
    Texture2D texture=LoadTexture(file.c_str());
    *texture_unique=texture;
    if(texture.id==0){
        safeLog(LOG_ERROR,"Failed to load texture: %s",file.c_str());
    }
    this->textures.push_back(std::move(texture_unique));
}
Button::~Button(){
    for(auto &texture:this->textures){
        if(texture->id!=0){
            UnloadTexture(*texture);
        }
    }
}
void Button::addTexture(Texture2D &texture){
    auto texture_unique=std::make_unique<Texture2D>();
    *texture_unique=texture;
    this->textures.push_back(std::move(texture_unique));
}
void Button::addTexture(std::string file){
    auto texture_unique=std::make_unique<Texture2D>();
    Texture2D texture=LoadTexture(file.c_str());
    *texture_unique=texture;
    if(texture.id==0){
        safeLog(LOG_ERROR,"Failed to load texture: %s",file.c_str());
    }
    this->textures.push_back(std::move(texture_unique));
}
bool Button::isHovered(){
    return CheckCollisionPointRec(mousePos,rect);
}
bool Button::isLeftClicked(){
    return isHovered()&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
bool Button::isRightClicked(){
    return isHovered()&&IsMouseButtonPressed(MOUSE_RIGHT_BUTTON);
}
void Button::draw(int index){
    if(index>=textures.size()){
        safeLog(LOG_ERROR,"Button index out of range");
        return;
    }
    if(textures[index]==nullptr){
        safeLog(LOG_ERROR,"Button texture is null");
        return;
    }
    if(textures[index]->id==0){
        safeLog(LOG_ERROR,"Button texture id is 0");
        return;
    }
    DrawTexture(*textures[index],rect.x,rect.y,WHITE);
}