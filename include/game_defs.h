#ifndef GAMEDEFS_H
#define GAMEDEFS_H

#include <string>
#include <SDL.h>
#include <SDL_image.h>


using namespace std;



#define GAMEWIDTH  700 
#define GAMEHEIGHT 500


#define PI 3.14159265


/*
A man wakes up for work
His child still sleeps 
It is necessary to be presentable
Breakfast is the most important part of the day
The weather is nice outside

He goes back to check on his child, work can wait for now.

He leaves for work, his child wakes in an empty home.

*/
const Uint32 TEXTUREFORMAT = SDL_PIXELFORMAT_RGBA8888;

const int NUM_SPRITESHEET = 5;
const int SS_PLAYER = 3;
const int SS_PLAYER_ACTION = 4;
const int SS_CHILD = 0;

//Actions in the form of sprite sheets
const string PLAYER_WALK = "./res/png/manwalk.png";

const string PLAYER_WAKE = "./res/png/manwake.png";

const string PLAYER_WASH = "./res/png/manwash.png";

//specific frame in player walk animation png which contains the idle frame
const int PLAYER_IDLE_FRAME = 18;

//player char states
//some textboxes use state strings as text display as well as state changes
//might be an issue with data coupling
const string STATE_ACTION = "ACTION";
const string STATE_NARRATION = "NARRATION";
const string STATE_IDLE = "IDLE";
const string STATE_LEFT = "Left";
const string STATE_RIGHT = "Right";



const int NUM_TEXTBOX = 5;
const int TB_NARRATION_BOX = 2;
const int TB_ACTION_BUTTON = 4;

const int NUM_TEXTURE = 5;
const int NUM_LINES = 10;

//Game Scenes
const string SCENE_INTRO = "INTRO";
const string SCENE_TRAN  = "TRAN";
const string SCENE_BEDROOM = "./res/png/bedroom.png";
const string SCENE_LIVINGROOM = "./res/png/livingroom.png";
const string SCENE_ENDDOOR = "./res/png/enddoor.png";


//Textures
const string TEX_DOOR = "./res/png/door.png";
const string TEX_SINK = "./res/png/sink.png";
const string TEX_CHILDSLEEP = "./res/png/childsleep.png";


//Texture Types
const string TTYPE_NORMAL = "NORM";
const string TTYPE_TRANSIT = "TRANSIT";
const string TTYPE_ACTION = "ACTION";

//Texture Actions
const string TACTION_END = "END";

//TextBox types
const string TBTYPE_NORM = "NORM";
const string TBTYPE_INPUT = "INPUT";


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

    SDL_Rect mColBoxOffset;

    string mType;
    string mName;
    string mButtonText;
    string mNarration;

    Uint32 mLastUpdate;
    bool mInteract;
};

struct SpriteSheet 
{
    bool mActive;
    bool mUpdate;
    SDL_Texture *mTexture;

    //Where to render the SpriteSheet
    //The w, and h
    SDL_Rect mDstRect;

    SDL_Rect mColBoxOffset;
    float mX;
    float mY;

    int mXOffset;
    int mWoffset;
    
    int mNumFrames;
    int mCurFrame;

    double mRotation;
    int mAlpha;
    SDL_Point *mCenter;
    SDL_RendererFlip mFlip;


    string mType;
    string mButtonText;
    string mNarration;

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
    string mType;
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

    Uint32 mStartTime;
    //If it's not set to 0, then after a duration set to inactive;
    Uint32 mDuration;

    Uint32 mLastUpdate;



    //if speed == -1, then Don't need to stagger render
    int mSpeed;
    int mNumRendered;
};

struct TextLine
{
    SDL_Texture *mFontTex;
    int mFontW;
    int mFontH;
    string mText;
    int mX, mY;
    int mAlpha;

    Uint32 mStartTime;
    Uint32 mDelay;
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

    int NarrativeCounter;

    int curStory;
    TextLine sStory[NUM_LINES];
};


#endif // GAMEDEFS_H

