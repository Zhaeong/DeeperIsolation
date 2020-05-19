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

    //handling frame updates
    Uint32 mLastUpdate;
    int mUpdateInterval;
    

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

struct TextBox
{
    SDL_Texture *mFontTex;
    int mFontW;
    int mFontH;

    SDL_Texture *mBoxTex;
    int mBoxDim;
    int mBoxMargin;
    bool mIsActive;

    string mText;

    int mX;
    int mY;


    //Denotes the number of characters that should be displayed 
    int mColumn;

    //Depends on mColumn, set when InitTextBox called
    int mRow;

    Uint32 mLastUpdate;

    //if speed == -1, then Don't need to stagger render
    int mSpeed;
    int mNumRendered;
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
    
    Uint32 startTime;
    Uint32 curTime;

    SDL_Texture *fontTexture;
    SDL_Texture *mainBoxTexture;

    TextBox startButton;

};


#endif // GAMEDEFS_H

