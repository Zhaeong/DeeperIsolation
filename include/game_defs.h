#ifndef GAMEDEFS_H
#define GAMEDEFS_H

#include <string>
#include <SDL.h>
#include <SDL_image.h>


using namespace std;



#define GAMEWIDTH  700 
#define GAMEHEIGHT 500


#define PI 3.14159265

const Uint32 TEXTUREFORMAT = SDL_PIXELFORMAT_RGBA8888;

const int NUM_SPRITESHEET = 5;
const int NUM_TEXTBOX = 5;
const int NUM_TEXTURE = 5;


//Game Scenes
const string SCENE_INTRO = "INTRO";
const string SCENE_TRAN  = "TRAN";
const string SCENE_START = "./res/png/bedroom.png";
const string SCENE_LIVINGROOM = "./res/png/livingroom.png";

//Texture Types
const string TTYPE_NORMAL = "NORM";
const string TTYPE_TRANSIT = "TRANSIT";

//Declared as extern so that it's shared between files
extern bool ShowDebug;

struct Texture
{
    bool mActive;
    int mX;
    int mY;
    int mW;
    int mH;
    int mRotation;
    int mAlpha;
    SDL_Texture *mTexture;
    SDL_Point *mCenter;
    SDL_RendererFlip mFlip;

    string mType;
    string mName;

    Uint32 mLastUpdate;
};

struct SpriteSheet 
{
    bool mActive;
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
    bool mActive;
    SDL_Texture *mFontTex;
    int mFontW;
    int mFontH;

    SDL_Texture *mBoxTex;
    int mBoxDim;
    int mBoxMargin;

    //0 for inactive
    //1 for hovered over
    //2 for pressed
    int mState;

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



struct LevelInfo
{
    bool mActive;
    Texture mLevelTex;
    SDL_Point mInitPlayerPos;

};


struct GameState
{
    string SceneCurrent;
    string SceneNext;
    SDL_Color screenColor;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_AudioDeviceID audioDevice;

    //SpriteSheet manSheet;


    string PlayerState;
    
    //Time controls
    Uint32 startTime;
    Uint32 curTime;

    //SDL Textures
    SDL_Texture *fontTexture;
    SDL_Texture *mainBoxTexture;

    //Level specific content
    Texture blackTex;
    LevelInfo lInfo;

    SpriteSheet ssArray[NUM_SPRITESHEET];
    TextBox tbArray[NUM_TEXTBOX];
    Texture tArray[NUM_TEXTURE];

};


#endif // GAMEDEFS_H

