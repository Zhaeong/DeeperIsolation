#ifndef GAMEDEFS_H
#define GAMEDEFS_H

#include <string>
#include <SDL.h>
#include <SDL_image.h>


using namespace std;



#define GAMEWIDTH  700 
#define GAMEHEIGHT 700


#define PI 3.14159265

const Uint32 TEXTUREFORMAT = SDL_PIXELFORMAT_RGBA8888;

//Declared as extern so that it's shared between files
extern bool ShowDebug;

struct Texture
{
    int mX;
    int mY;
    int mW;
    int mH;
    int mRotation;
    int mAlpha;
    SDL_Texture *mTexture;
    SDL_Point *mCenter;
    SDL_RendererFlip mFlip;
};

struct SpriteSheet 
{
    SDL_Texture *mTexture;

    //Where to render the SpriteSheet
    //The w, and h
    SDL_Rect mDstRect;
    
    int mNumFrames;
    int mCurFrame;

    double mRotation;
    int mAlpha;
    SDL_Point *mCenter;
    SDL_RendererFlip mFlip;
    

};

struct vect2
{
    float x;
    float y;
};

struct AudioClip
{
    string wavPath;
    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8 *wavBuffer;
};



struct GameState
{
    string State;
    SDL_Color screenColor;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_AudioDeviceID audioDevice;

    SpriteSheet manSheet;

    string PlayerState;

};




#endif // GAMEDEFS_H

